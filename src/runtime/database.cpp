#include "s/database.hpp"
#include "s/error.hpp"

#include <chrono>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

namespace s {
namespace {

struct RemoteDatabase {
  std::string adapter;
  std::string endpoint;
  std::string database;
  std::string collection;
};

std::shared_ptr<CallableData> callable(std::string name,std::size_t min,std::size_t max,
    std::function<Value(const std::vector<Value>&,SourcePos)> fn,bool variadic=false){
  auto c=std::make_shared<CallableData>();
  c->name=std::move(name);c->min_args=min;c->max_args=max;c->variadic=variadic;c->call=std::move(fn);
  return c;
}

std::shared_ptr<FunctionSig> sig(std::vector<TypeInfo> params,TypeInfo result,bool variadic=false,std::size_t min=0,bool fallible=false){
  auto s=std::make_shared<FunctionSig>();s->params=std::move(params);s->result=std::move(result);s->variadic=variadic;s->min_args=min;s->fallible=fallible;return s;
}

TypeInfo fn(std::vector<TypeInfo> p,TypeInfo r,bool variadic=false,std::size_t min=0,bool fallible=false){
  TypeInfo t(TypeKind::Function);t.callable=sig(std::move(p),std::move(r),variadic,min,fallible);return t;
}

TypeInfo handle_type(std::string name){TypeInfo t(TypeKind::NativeHandle);t.name=std::move(name);return t;}

std::string text(const Value& v,SourcePos p,const std::string& name){
  if(auto s=std::get_if<std::string>(&v.data()))return *s;
  throw Error(p,name+" needs Text.");
}

template<class T> Value handle(std::string tag,std::shared_ptr<T> value){
  auto h=std::make_shared<NativeHandleData>();h->tag=std::move(tag);h->resource=std::move(value);return Value(h);
}

template<class T> std::shared_ptr<T> as_handle(const Value& v,const std::string& tag,SourcePos p,const std::string& name){
  auto h=std::get_if<std::shared_ptr<NativeHandleData>>(&v.data());
  if(!h||!(*h)||(*h)->tag!=tag||!(*h)->resource)throw Error(p,name+" needs "+tag+".");
  return std::static_pointer_cast<T>((*h)->resource);
}

std::string json_escape(const std::string& s){
  std::ostringstream out;out<<'"';
  for(unsigned char c:s){
    switch(c){
      case '"':out<<"\\\"";break;case '\\':out<<"\\\\";break;
      case '\b':out<<"\\b";break;case '\f':out<<"\\f";break;case '\n':out<<"\\n";break;
      case '\r':out<<"\\r";break;case '\t':out<<"\\t";break;
      default:if(c<0x20){static const char* hex="0123456789abcdef";out<<"\\u00"<<hex[c>>4]<<hex[c&15];}else out<<static_cast<char>(c);
    }
  }
  out<<'"';return out.str();
}

std::string shell_quote(const std::string& s){
#ifdef _WIN32
  std::string out="\"";for(char c:s){if(c=='\"')out+="\\\"";else out+=c;}return out+"\"";
#else
  std::string out="'";for(char c:s){if(c=='\'')out+="'\\''";else out+=c;}return out+"'";
#endif
}

std::string command_output(const std::string& command,SourcePos p,const std::string& kind){
#ifdef _WIN32
  FILE* pipe=_popen(command.c_str(),"r");
#else
  FILE* pipe=popen(command.c_str(),"r");
#endif
  if(!pipe)throw RuntimeFailure({"Could not start external database transport.","",p.line,kind});
  std::string out;char buffer[4096];while(std::fgets(buffer,sizeof(buffer),pipe))out+=buffer;
#ifdef _WIN32
  int code=_pclose(pipe);
#else
  int code=pclose(pipe);
#endif
  if(code!=0)throw RuntimeFailure({"External database operation failed. Check the adapter dependency, credentials, endpoint, and payload.","",p.line,kind});
  return out;
}

struct TempFiles {
  std::vector<std::filesystem::path> paths;
  ~TempFiles(){for(const auto& path:paths){std::error_code ec;std::filesystem::remove(path,ec);}}
};

std::filesystem::path temp_path(const std::string& suffix){
  auto stamp=std::chrono::high_resolution_clock::now().time_since_epoch().count();
  return std::filesystem::temp_directory_path()/("se-db-"+std::to_string(stamp)+suffix);
}

void private_file(const std::filesystem::path& path){
#ifndef _WIN32
  std::error_code ec;
  std::filesystem::permissions(path,std::filesystem::perms::owner_read|std::filesystem::perms::owner_write,std::filesystem::perm_options::replace,ec);
#else
  (void)path;
#endif
}

std::string mongo_exec(const RemoteDatabase& db,const std::string& action,const std::string& payload,SourcePos p){
  auto request=temp_path(".json");auto script=temp_path(".cjs");TempFiles cleanup{{request,script}};
  {
    std::ofstream out(request,std::ios::binary|std::ios::trunc);
    if(!out)throw RuntimeFailure({"Could not create a temporary MongoDB request.","",p.line,"DatabaseError"});
    out<<"{\"uri\":"<<json_escape(db.endpoint)<<",\"database\":"<<json_escape(db.database)
       <<",\"collection\":"<<json_escape(db.collection)<<",\"action\":"<<json_escape(action)
       <<",\"payload\":"<<json_escape(payload)<<"}";
  }
  private_file(request);
  {
    std::ofstream out(script,std::ios::binary|std::ios::trunc);
    if(!out)throw RuntimeFailure({"Could not create the MongoDB adapter helper.","",p.line,"DatabaseError"});
    out<<R"JS(const fs=require('fs');
const req=JSON.parse(fs.readFileSync(process.argv[2],'utf8'));
let MongoClient;
try{({MongoClient}=require(require.resolve('mongodb',{paths:[process.cwd()]})));}
catch(e){console.error('SE MongoDB adapter needs the official mongodb Node package. Run: npm install mongodb');process.exit(71);}
(async()=>{
  const client=new MongoClient(req.uri);
  try{
    await client.connect();
    const collection=client.db(req.database).collection(req.collection);
    const payload=req.payload?JSON.parse(req.payload):{};
    let result;
    if(req.action==='find_one') result=await collection.findOne(payload);
    else if(req.action==='find_many') result=await collection.find(payload).toArray();
    else if(req.action==='insert_one'){const r=await collection.insertOne(payload);result={insertedId:r.insertedId};}
    else if(req.action==='insert_many'){const r=await collection.insertMany(payload);result={insertedCount:r.insertedCount,insertedIds:r.insertedIds};}
    else if(req.action==='delete_one'){const r=await collection.deleteOne(payload);result={deletedCount:r.deletedCount};}
    else if(req.action==='delete_many'){const r=await collection.deleteMany(payload);result={deletedCount:r.deletedCount};}
    else if(req.action==='count') result={count:await collection.countDocuments(payload)};
    else if(req.action==='update_one'){
      if(!payload||typeof payload!=='object'||!payload.filter||!payload.update)throw new Error('update_one payload needs filter and update');
      const r=await collection.updateOne(payload.filter,payload.update,payload.options||{});
      result={matchedCount:r.matchedCount,modifiedCount:r.modifiedCount,upsertedId:r.upsertedId||null};
    }else if(req.action==='replace_one'){
      if(!payload||typeof payload!=='object'||!payload.filter||!payload.replacement)throw new Error('replace_one payload needs filter and replacement');
      const r=await collection.replaceOne(payload.filter,payload.replacement,payload.options||{});
      result={matchedCount:r.matchedCount,modifiedCount:r.modifiedCount,upsertedId:r.upsertedId||null};
    }else throw new Error(`Unsupported MongoDB action: ${req.action}`);
    process.stdout.write(JSON.stringify(result));
  }finally{await client.close();}
})().catch(e=>{console.error(e&&e.message?e.message:String(e));process.exit(72);});
)JS";
  }
  private_file(script);
  auto command=std::string("node ")+shell_quote(script.string())+" "+shell_quote(request.string());
  return command_output(command,p,"MongoDBError");
}

std::string gas_exec(const RemoteDatabase& db,const std::string& action,const std::string& payload,SourcePos p){
  auto request=temp_path(".json");TempFiles cleanup{{request}};
  {
    std::ofstream out(request,std::ios::binary|std::ios::trunc);
    if(!out)throw RuntimeFailure({"Could not create a temporary Google Apps Script request.","",p.line,"DatabaseError"});
    out<<"{\"action\":"<<json_escape(action)<<",\"payload\":"<<json_escape(payload)<<"}";
  }
  private_file(request);
  std::string command="curl --fail --silent --show-error --location --request POST --header "+shell_quote("Content-Type: application/json")+" --data-binary "+shell_quote("@"+request.string())+" "+shell_quote(db.endpoint);
  return command_output(command,p,"GasDatabaseError");
}

} // namespace

void extend_database_type(TypeInfo& module){
  TypeInfo text_t(TypeKind::Text),remote=handle_type("RemoteDatabase");
  module.members["connect"]=fn({text_t,text_t,text_t,text_t},remote,true,2,true);
  module.members["exec"]=fn({remote,text_t,text_t},text_t,false,0,true);
  module.members["adapter"]=fn({remote},text_t);
}

void extend_database_module(const std::shared_ptr<ModuleData>& module,Interpreter&){
  module->exports["connect"]=callable("db.connect",2,4,[](const std::vector<Value>& a,SourcePos p){
    auto adapter=text(a[0],p,"db.connect");
    auto remote=std::make_shared<RemoteDatabase>();remote->adapter=adapter;
    if(adapter=="mongodb"||adapter=="mongo"){
      if(a.size()!=4)throw Error(p,"db.connect mongodb needs adapter, URI, database, and collection.","Example: db.connect \"mongodb\" uri \"app\" \"users\"");
      remote->adapter="mongodb";remote->endpoint=text(a[1],p,"db.connect");remote->database=text(a[2],p,"db.connect");remote->collection=text(a[3],p,"db.connect");
      if(remote->endpoint.rfind("mongodb://",0)!=0&&remote->endpoint.rfind("mongodb+srv://",0)!=0)throw Error(p,"MongoDB URI must start with mongodb:// or mongodb+srv://.");
    }else if(adapter=="gas"||adapter=="google-apps-script"){
      if(a.size()!=2)throw Error(p,"db.connect gas needs adapter and HTTPS Web App URL.","Example: db.connect \"gas\" \"https://script.google.com/macros/s/.../exec\"");
      remote->adapter="gas";remote->endpoint=text(a[1],p,"db.connect");
      if(remote->endpoint.rfind("https://",0)!=0)throw Error(p,"Google Apps Script database URL must use https://.");
    }else throw Error(p,"Unknown database adapter '"+adapter+"'.","Use mongodb or gas.");
    return handle("RemoteDatabase",remote);
  },true);
  module->exports["exec"]=callable("db.exec",3,3,[](const std::vector<Value>& a,SourcePos p){
    auto remote=as_handle<RemoteDatabase>(a[0],"RemoteDatabase",p,"db.exec");
    auto action=text(a[1],p,"db.exec"),payload=text(a[2],p,"db.exec");
    if(remote->adapter=="mongodb")return Value(mongo_exec(*remote,action,payload,p));
    if(remote->adapter=="gas")return Value(gas_exec(*remote,action,payload,p));
    throw Error(p,"Unknown remote database adapter '"+remote->adapter+"'.");
  });
  module->exports["adapter"]=callable("db.adapter",1,1,[](const std::vector<Value>& a,SourcePos p){return Value(as_handle<RemoteDatabase>(a[0],"RemoteDatabase",p,"db.adapter")->adapter);});
}

} // namespace s
