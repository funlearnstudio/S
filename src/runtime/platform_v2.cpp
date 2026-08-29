#include "s/platform.hpp"
#include "s/interpreter.hpp"
#include "s/error.hpp"

#include <algorithm>
#include <atomic>
#include <cctype>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <future>
#include <iomanip>
#include <map>
#include <memory>
#include <mutex>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#endif

namespace s {
namespace {

std::shared_ptr<CallableData> callable(std::string name, std::size_t min, std::size_t max,
    std::function<Value(const std::vector<Value>&, SourcePos)> fn, bool variadic=false) {
  auto c=std::make_shared<CallableData>();
  c->name=std::move(name);
  c->min_args=min;
  c->max_args=max;
  c->variadic=variadic;
  c->call=std::move(fn);
  return c;
}

std::shared_ptr<FunctionSig> sig(std::vector<TypeInfo> params, TypeInfo result,
    bool variadic=false, std::size_t min=0, bool fallible=false) {
  auto s=std::make_shared<FunctionSig>();
  s->params=std::move(params);
  s->result=std::move(result);
  s->variadic=variadic;
  s->min_args=min;
  s->fallible=fallible;
  return s;
}

TypeInfo fn_type(std::vector<TypeInfo> params, TypeInfo result,
    bool variadic=false, std::size_t min=0, bool fallible=false) {
  TypeInfo t(TypeKind::Function);
  t.callable=sig(std::move(params),std::move(result),variadic,min,fallible);
  return t;
}

TypeInfo module_type(const std::string& name) {
  TypeInfo t(TypeKind::Module);
  t.name=name;
  return t;
}

TypeInfo list_type(TypeInfo element=TypeInfo{}) {
  TypeInfo t(TypeKind::List);
  t.element=std::make_shared<TypeInfo>(std::move(element));
  return t;
}

std::string require_text(const Value& v, SourcePos p, const std::string& name) {
  if(auto s=std::get_if<std::string>(&v.data())) return *s;
  throw Error(p,name+" needs Text.");
}

std::int64_t require_int(const Value& v, SourcePos p, const std::string& name) {
  if(auto n=std::get_if<std::int64_t>(&v.data())) return *n;
  throw Error(p,name+" needs Int.");
}

bool require_bool(const Value& v, SourcePos p, const std::string& name) {
  if(auto b=std::get_if<bool>(&v.data())) return *b;
  throw Error(p,name+" needs Bool.");
}

std::shared_ptr<ListData> require_list(const Value& v, SourcePos p, const std::string& name) {
  if(auto l=std::get_if<std::shared_ptr<ListData>>(&v.data())) return *l;
  throw Error(p,name+" needs a List.");
}

std::string lower_ascii(std::string s) {
  std::transform(s.begin(),s.end(),s.begin(),[](unsigned char c){return static_cast<char>(std::tolower(c));});
  return s;
}

std::string trim_copy(std::string s) {
  auto ws=[](unsigned char c){return std::isspace(c)!=0;};
  auto first=std::find_if_not(s.begin(),s.end(),ws);
  auto last=std::find_if_not(s.rbegin(),s.rend(),ws).base();
  if(first>=last) return {};
  return std::string(first,last);
}

std::string shell_quote(const std::string& s) {
#ifdef _WIN32
  std::string out="\"";
  for(char c:s){if(c=='\"')out+="\\\"";else out+=c;}
  return out+"\"";
#else
  std::string out="'";
  for(char c:s){if(c=='\'')out+="'\\''";else out+=c;}
  return out+"'";
#endif
}

int normalized_system(const std::string& command) {
  int code=std::system(command.c_str());
#ifndef _WIN32
  if(code!=-1&&WIFEXITED(code)) return WEXITSTATUS(code);
#endif
  return code;
}

std::string process_output(const std::string& command, SourcePos p) {
#ifdef _WIN32
  FILE* pipe=_popen(command.c_str(),"r");
#else
  FILE* pipe=popen(command.c_str(),"r");
#endif
  if(!pipe) throw RuntimeFailure({"Could not start process.","",p.line,"ProcessError"});
  std::string out;
  char buf[4096];
  while(std::fgets(buf,sizeof(buf),pipe)) out+=buf;
#ifdef _WIN32
  _pclose(pipe);
#else
  pclose(pipe);
#endif
  return out;
}

std::string json_escape(const std::string& s) {
  std::ostringstream out;
  out<<'"';
  for(unsigned char c:s){
    switch(c){
      case '"':out<<"\\\"";break;
      case '\\':out<<"\\\\";break;
      case '\b':out<<"\\b";break;
      case '\f':out<<"\\f";break;
      case '\n':out<<"\\n";break;
      case '\r':out<<"\\r";break;
      case '\t':out<<"\\t";break;
      default:
        if(c<0x20) out<<"\\u"<<std::hex<<std::setw(4)<<std::setfill('0')<<static_cast<int>(c)<<std::dec;
        else out<<static_cast<char>(c);
    }
  }
  out<<'"';
  return out.str();
}

std::string json_stringify_impl(const Value& v, int indent, int depth) {
  auto pad=[&](int extra=0){return std::string(static_cast<std::size_t>((depth+extra)*indent),' ');};
  if(std::holds_alternative<std::monostate>(v.data())) return "null";
  if(auto n=std::get_if<std::int64_t>(&v.data())) return std::to_string(*n);
  if(auto n=std::get_if<double>(&v.data())){std::ostringstream o;o<<std::setprecision(15)<<*n;return o.str();}
  if(auto b=std::get_if<bool>(&v.data())) return *b?"true":"false";
  if(auto s=std::get_if<std::string>(&v.data())) return json_escape(*s);
  if(auto p=std::get_if<PathData>(&v.data())) return json_escape(p->path.string());
  if(auto l=std::get_if<std::shared_ptr<ListData>>(&v.data())){
    if((*l)->items.empty()) return "[]";
    std::ostringstream o;
    o<<'[';
    for(std::size_t i=0;i<(*l)->items.size();++i){
      if(i) o<<',';
      if(indent) o<<'\n'<<std::string(static_cast<std::size_t>((depth+1)*indent),' ');
      o<<json_stringify_impl((*l)->items[i],indent,depth+1);
    }
    if(indent) o<<'\n'<<pad();
    o<<']';
    return o.str();
  }
  if(auto s=std::get_if<std::shared_ptr<SetData>>(&v.data())){
    auto l=std::make_shared<ListData>();
    l->items=(*s)->items;
    return json_stringify_impl(Value(l),indent,depth);
  }
  if(auto m=std::get_if<std::shared_ptr<MapData>>(&v.data())){
    if((*m)->items.empty()) return "{}";
    std::ostringstream o;
    o<<'{';
    for(std::size_t i=0;i<(*m)->items.size();++i){
      if(i) o<<',';
      if(indent) o<<'\n'<<std::string(static_cast<std::size_t>((depth+1)*indent),' ');
      o<<json_escape((*m)->items[i].first)<<':'<<(indent?" ":"")<<json_stringify_impl((*m)->items[i].second,indent,depth+1);
    }
    if(indent) o<<'\n'<<pad();
    o<<'}';
    return o.str();
  }
  if(auto o=std::get_if<std::shared_ptr<ObjectData>>(&v.data())){
    auto m=std::make_shared<MapData>();
    for(auto&[k,x]:(*o)->fields) m->items.emplace_back(k,x);
    return json_stringify_impl(Value(m),indent,depth);
  }
  return json_escape(v.text());
}

std::string json_stringify(const Value& v, int indent=0) {
  return json_stringify_impl(v,indent,0);
}

class JsonParser {
public:
  JsonParser(std::string text, SourcePos p):text_(std::move(text)),pos_(p){}
  Value parse(){skip();auto v=value();skip();if(i_!=text_.size())fail("Unexpected text after JSON value.");return v;}
private:
  std::string text_;
  std::size_t i_=0;
  SourcePos pos_;
  [[noreturn]] void fail(const std::string& m) const{throw RuntimeFailure({"JSON: "+m,"",pos_.line,"JsonError"});}
  void skip(){while(i_<text_.size()&&std::isspace(static_cast<unsigned char>(text_[i_])))++i_;}
  bool take(char c){skip();if(i_<text_.size()&&text_[i_]==c){++i_;return true;}return false;}
  void expect(char c){if(!take(c))fail(std::string("Expected '")+c+"'.");}
  Value value(){
    skip();
    if(i_>=text_.size()) fail("Unexpected end of input.");
    char c=text_[i_];
    if(c=='"') return string();
    if(c=='[') return array();
    if(c=='{') return object();
    if(c=='t'&&text_.compare(i_,4,"true")==0){i_+=4;return true;}
    if(c=='f'&&text_.compare(i_,5,"false")==0){i_+=5;return false;}
    if(c=='n'&&text_.compare(i_,4,"null")==0){i_+=4;return Value{};}
    if(c=='-'||std::isdigit(static_cast<unsigned char>(c))) return number();
    fail("Invalid value.");
  }
  Value number(){
    auto start=i_;
    if(text_[i_]=='-') ++i_;
    if(i_>=text_.size()) fail("Invalid number.");
    if(text_[i_]=='0') ++i_;
    else{
      if(!std::isdigit(static_cast<unsigned char>(text_[i_]))) fail("Invalid number.");
      while(i_<text_.size()&&std::isdigit(static_cast<unsigned char>(text_[i_]))) ++i_;
    }
    bool real=false;
    if(i_<text_.size()&&text_[i_]=='.'){
      real=true;++i_;
      if(i_>=text_.size()||!std::isdigit(static_cast<unsigned char>(text_[i_]))) fail("Invalid decimal.");
      while(i_<text_.size()&&std::isdigit(static_cast<unsigned char>(text_[i_]))) ++i_;
    }
    if(i_<text_.size()&&(text_[i_]=='e'||text_[i_]=='E')){
      real=true;++i_;
      if(i_<text_.size()&&(text_[i_]=='+'||text_[i_]=='-')) ++i_;
      if(i_>=text_.size()||!std::isdigit(static_cast<unsigned char>(text_[i_]))) fail("Invalid exponent.");
      while(i_<text_.size()&&std::isdigit(static_cast<unsigned char>(text_[i_]))) ++i_;
    }
    auto part=text_.substr(start,i_-start);
    try{if(real)return std::stod(part);return static_cast<std::int64_t>(std::stoll(part));}
    catch(...){fail("Number is out of range.");}
  }
  std::string string(){
    expect('"');
    std::string out;
    while(i_<text_.size()){
      char c=text_[i_++];
      if(c=='"') return out;
      if(c!='\\'){out+=c;continue;}
      if(i_>=text_.size()) fail("Broken string escape.");
      char e=text_[i_++];
      switch(e){
        case '"':out+='"';break;
        case '\\':out+='\\';break;
        case '/':out+='/';break;
        case 'b':out+='\b';break;
        case 'f':out+='\f';break;
        case 'n':out+='\n';break;
        case 'r':out+='\r';break;
        case 't':out+='\t';break;
        case 'u':{
          if(i_+4>text_.size()) fail("Broken unicode escape.");
          unsigned code=0;
          for(int k=0;k<4;++k){
            char h=text_[i_++];code<<=4;
            if(h>='0'&&h<='9')code+=h-'0';
            else if(h>='a'&&h<='f')code+=10+h-'a';
            else if(h>='A'&&h<='F')code+=10+h-'A';
            else fail("Broken unicode escape.");
          }
          if(code<=0x7f) out+=static_cast<char>(code);
          else if(code<=0x7ff){out+=static_cast<char>(0xc0|(code>>6));out+=static_cast<char>(0x80|(code&0x3f));}
          else{out+=static_cast<char>(0xe0|(code>>12));out+=static_cast<char>(0x80|((code>>6)&0x3f));out+=static_cast<char>(0x80|(code&0x3f));}
          break;
        }
        default:fail("Unknown string escape.");
      }
    }
    fail("Unterminated string.");
  }
  Value array(){
    expect('[');
    auto out=std::make_shared<ListData>();
    skip();
    if(take(']')) return out;
    while(true){out->items.push_back(value());skip();if(take(']'))break;expect(',');}
    return out;
  }
  Value object(){
    expect('{');
    auto out=std::make_shared<MapData>();
    skip();
    if(take('}')) return out;
    while(true){
      skip();
      if(i_>=text_.size()||text_[i_]!='"') fail("Object keys must be strings.");
      auto key=string();
      expect(':');
      auto v=value();
      out->items.emplace_back(std::move(key),std::move(v));
      skip();
      if(take('}')) break;
      expect(',');
    }
    return out;
  }
};

std::shared_ptr<ObjectData> tagged_object(const std::string& name) {
  auto o=std::make_shared<ObjectData>();
  o->type=std::make_shared<TypeData>();
  o->type->name=name;
  return o;
}

std::shared_ptr<ObjectData> response_object(std::int64_t status, std::string body, std::string type) {
  auto o=tagged_object("WebResponse");
  o->fields["status"]=Value(status);
  o->fields["body"]=Value(std::move(body));
  o->fields["type"]=Value(std::move(type));
  return o;
}

std::shared_ptr<ObjectData> option_object(bool has, Value value={}) {
  auto o=tagged_object("Option");
  o->fields["has"]=Value(has);
  o->fields["value"]=std::move(value);
  return o;
}

std::shared_ptr<ObjectData> result_object(bool ok, Value value, std::string error) {
  auto o=tagged_object("Result");
  o->fields["ok"]=Value(ok);
  o->fields["value"]=std::move(value);
  o->fields["error"]=Value(std::move(error));
  return o;
}

std::shared_ptr<ObjectData> require_tagged(const Value& v, const std::string& tag, SourcePos p) {
  auto o=std::get_if<std::shared_ptr<ObjectData>>(&v.data());
  if(!o||!(*o)->type||(*o)->type->name!=tag) throw Error(p,"Expected "+tag+" value.");
  return *o;
}

struct HttpResponse {
  int status=200;
  std::string body;
  std::string type="text/plain; charset=utf-8";
  std::unordered_map<std::string,std::string> headers;
};

HttpResponse response_from_value(const Value& v) {
  if(auto s=std::get_if<std::string>(&v.data())) return {200,*s,"text/plain; charset=utf-8",{}};
  if(auto o=std::get_if<std::shared_ptr<ObjectData>>(&v.data())){
    if((*o)->type&&(*o)->type->name=="WebResponse"){
      HttpResponse r;
      auto st=(*o)->fields.find("status"),b=(*o)->fields.find("body"),t=(*o)->fields.find("type");
      if(st!=(*o)->fields.end()&&std::holds_alternative<std::int64_t>(st->second.data())) r.status=static_cast<int>(std::get<std::int64_t>(st->second.data()));
      if(b!=(*o)->fields.end()) r.body=b->second.text();
      if(t!=(*o)->fields.end()) r.type=t->second.text();
      return r;
    }
  }
  if(std::holds_alternative<std::shared_ptr<MapData>>(v.data())||std::holds_alternative<std::shared_ptr<ListData>>(v.data())) return {200,json_stringify(v),"application/json; charset=utf-8",{}};
  return {200,v.text(),"text/plain; charset=utf-8",{}};
}

#ifdef _WIN32
using Socket=SOCKET;
constexpr Socket invalid_socket=INVALID_SOCKET;
struct WinsockInit{WinsockInit(){WSADATA d{};if(WSAStartup(MAKEWORD(2,2),&d)!=0)throw std::runtime_error("WSAStartup failed");}~WinsockInit(){WSACleanup();}};
void ensure_sockets(){static WinsockInit init;(void)init;}
void close_socket(Socket s){if(s!=invalid_socket)closesocket(s);}
#else
using Socket=int;
constexpr Socket invalid_socket=-1;
void ensure_sockets(){}
void close_socket(Socket s){if(s!=invalid_socket)::close(s);}
#endif

struct SocketGuard{
  Socket value=invalid_socket;
  explicit SocketGuard(Socket s=invalid_socket):value(s){}
  ~SocketGuard(){close_socket(value);}
  SocketGuard(const SocketGuard&)=delete;
  SocketGuard& operator=(const SocketGuard&)=delete;
};

bool send_all(Socket s, const std::string& data) {
  std::size_t sent=0;
  while(sent<data.size()){
#ifdef _WIN32
    int n=send(s,data.data()+sent,static_cast<int>(data.size()-sent),0);
#else
    auto n=send(s,data.data()+sent,data.size()-sent,0);
#endif
    if(n<=0) return false;
    sent+=static_cast<std::size_t>(n);
  }
  return true;
}

std::string recv_all(Socket s) {
  std::string out;
  char buf[8192];
  while(true){
#ifdef _WIN32
    int n=recv(s,buf,sizeof(buf),0);
#else
    auto n=recv(s,buf,sizeof(buf),0);
#endif
    if(n<=0) break;
    out.append(buf,buf+n);
  }
  return out;
}

struct ParsedUrl{std::string host;std::string port="80";std::string target="/";};

ParsedUrl parse_http_url(const std::string& url, SourcePos p) {
  const std::string prefix="http://";
  if(url.rfind(prefix,0)!=0) throw Error(p,"Expected an http:// URL for the built-in socket transport.");
  auto rest=url.substr(prefix.size());
  auto slash=rest.find('/');
  auto authority=slash==std::string::npos?rest:rest.substr(0,slash);
  auto target=slash==std::string::npos?"/":rest.substr(slash);
  if(authority.empty()) throw Error(p,"HTTP URL has no host.");
  ParsedUrl out;
  out.target=target;
  auto colon=authority.rfind(':');
  if(colon!=std::string::npos&&authority.find(':')==colon){out.host=authority.substr(0,colon);out.port=authority.substr(colon+1);}
  else out.host=authority;
  if(out.host.empty()||out.port.empty()) throw Error(p,"HTTP URL is invalid.");
  return out;
}

HttpResponse curl_request(const std::string& method, const std::string& url,
    const std::string& body, const std::string& content_type, SourcePos p) {
  std::string command="curl -sS -L -X "+shell_quote(method)+" -H "+shell_quote("Content-Type: "+content_type);
  if(!body.empty()||method=="POST"||method=="PUT"||method=="PATCH") command+=" --data-binary "+shell_quote(body);
  command+=" -w "+shell_quote("\n__SE_HTTP_META__:%{http_code}:%{content_type}")+" "+shell_quote(url);
  auto raw=process_output(command,p);
  const std::string marker="\n__SE_HTTP_META__:";
  auto pos=raw.rfind(marker);
  if(pos==std::string::npos) throw RuntimeFailure({"HTTPS request failed. SE uses the system curl command for TLS; make sure curl is installed.","",p.line,"HttpError"});
  auto meta=raw.substr(pos+marker.size());
  raw.resize(pos);
  auto colon=meta.find(':');
  if(colon==std::string::npos) throw RuntimeFailure({"HTTPS response metadata was invalid.","",p.line,"HttpError"});
  HttpResponse out;
  try{out.status=std::stoi(meta.substr(0,colon));}catch(...){throw RuntimeFailure({"HTTPS status code was invalid.","",p.line,"HttpError"});}
  out.type=trim_copy(meta.substr(colon+1));
  if(out.type.empty()) out.type="application/octet-stream";
  out.body=std::move(raw);
  return out;
}

HttpResponse http_client_request(const std::string& method, const std::string& url,
    const std::string& body, const std::string& content_type, SourcePos p) {
  if(url.rfind("https://",0)==0) return curl_request(method,url,body,content_type,p);
  ensure_sockets();
  auto u=parse_http_url(url,p);
  addrinfo hints{};
  hints.ai_family=AF_UNSPEC;
  hints.ai_socktype=SOCK_STREAM;
  addrinfo* result=nullptr;
  if(getaddrinfo(u.host.c_str(),u.port.c_str(),&hints,&result)!=0) throw RuntimeFailure({"Could not resolve HTTP host '"+u.host+"'.","",p.line,"HttpError"});
  Socket sock=invalid_socket;
  for(auto* a=result;a;a=a->ai_next){
    sock=socket(a->ai_family,a->ai_socktype,a->ai_protocol);
    if(sock==invalid_socket) continue;
    if(connect(sock,a->ai_addr,static_cast<int>(a->ai_addrlen))==0) break;
    close_socket(sock);
    sock=invalid_socket;
  }
  freeaddrinfo(result);
  if(sock==invalid_socket) throw RuntimeFailure({"Could not connect to "+u.host+":"+u.port+".","",p.line,"HttpError"});
  SocketGuard guard(sock);
  std::ostringstream req;
  req<<method<<' '<<u.target<<" HTTP/1.1\r\nHost: "<<u.host<<"\r\nUser-Agent: SE/0.6\r\nAccept: */*\r\nConnection: close\r\n";
  if(!body.empty()||method=="POST"||method=="PUT"||method=="PATCH") req<<"Content-Type: "<<content_type<<"\r\nContent-Length: "<<body.size()<<"\r\n";
  req<<"\r\n"<<body;
  if(!send_all(sock,req.str())) throw RuntimeFailure({"HTTP send failed.","",p.line,"HttpError"});
  auto raw=recv_all(sock);
  auto head_end=raw.find("\r\n\r\n");
  if(head_end==std::string::npos) throw RuntimeFailure({"HTTP response was incomplete.","",p.line,"HttpError"});
  auto head=raw.substr(0,head_end);
  HttpResponse out;
  out.body=raw.substr(head_end+4);
  std::istringstream in(head);
  std::string line;
  if(!std::getline(in,line)) throw RuntimeFailure({"HTTP response has no status line.","",p.line,"HttpError"});
  {std::istringstream s(line);std::string version;s>>version>>out.status;}
  while(std::getline(in,line)){
    if(!line.empty()&&line.back()=='\r') line.pop_back();
    auto c=line.find(':');
    if(c==std::string::npos) continue;
    auto k=lower_ascii(trim_copy(line.substr(0,c)));
    auto v=trim_copy(line.substr(c+1));
    out.headers[k]=v;
    if(k=="content-type") out.type=v;
  }
  return out;
}

std::string status_text(int status) {
  switch(status){
    case 200:return "OK";
    case 201:return "Created";
    case 204:return "No Content";
    case 400:return "Bad Request";
    case 404:return "Not Found";
    case 405:return "Method Not Allowed";
    case 500:return "Internal Server Error";
    default:return "OK";
  }
}

struct WebRequest{std::string method,path,query,body;std::unordered_map<std::string,std::string> headers,params;};
struct WebRoute{std::string method,pattern;Value handler;};
struct WebState{std::vector<WebRoute> routes;WebRequest current;};

bool route_match(const std::string& pattern, const std::string& path,
    std::unordered_map<std::string,std::string>& params) {
  auto split=[](const std::string&s){
    std::vector<std::string> v;
    std::size_t i=0;
    while(i<s.size()){
      while(i<s.size()&&s[i]=='/') ++i;
      if(i>=s.size()) break;
      auto j=s.find('/',i);
      if(j==std::string::npos) j=s.size();
      v.push_back(s.substr(i,j-i));
      i=j;
    }
    return v;
  };
  auto a=split(pattern),b=split(path);
  if(a.size()!=b.size()) return false;
  params.clear();
  for(std::size_t i=0;i<a.size();++i){
    if(!a[i].empty()&&a[i][0]==':') params[a[i].substr(1)]=b[i];
    else if(a[i]!=b[i]) return false;
  }
  return true;
}

HttpResponse dispatch_web(WebState& state, Interpreter& vm, const std::string& method,
    const std::string& target, const std::string& body,
    const std::unordered_map<std::string,std::string>& headers, SourcePos p) {
  auto q=target.find('?');
  state.current.method=method;
  state.current.path=q==std::string::npos?target:target.substr(0,q);
  state.current.query=q==std::string::npos?"":target.substr(q+1);
  state.current.body=body;
  state.current.headers=headers;
  state.current.params.clear();
  for(auto& route:state.routes){
    std::unordered_map<std::string,std::string> params;
    if(route.method!=method||!route_match(route.pattern,state.current.path,params)) continue;
    state.current.params=std::move(params);
    try{return response_from_value(vm.invoke(route.handler,{Value(body)},p));}
    catch(const RuntimeFailure&e){return {500,json_stringify(Value(std::string(e.what()))),"application/json; charset=utf-8",{}};}
    catch(const std::exception&e){return {500,e.what(),"text/plain; charset=utf-8",{}};}
  }
  return {404,"Not Found","text/plain; charset=utf-8",{}};
}

void serve_web(WebState& state, Interpreter& vm, int port, SourcePos p) {
  ensure_sockets();
  Socket server=socket(AF_INET,SOCK_STREAM,0);
  if(server==invalid_socket) throw RuntimeFailure({"Could not create HTTP server socket.","",p.line,"HttpError"});
  SocketGuard server_guard(server);
  int yes=1;
#ifdef _WIN32
  setsockopt(server,SOL_SOCKET,SO_REUSEADDR,reinterpret_cast<const char*>(&yes),sizeof(yes));
#else
  setsockopt(server,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(yes));
#endif
  sockaddr_in addr{};
  addr.sin_family=AF_INET;
  addr.sin_addr.s_addr=htonl(INADDR_ANY);
  addr.sin_port=htons(static_cast<unsigned short>(port));
  if(bind(server,reinterpret_cast<sockaddr*>(&addr),sizeof(addr))!=0) throw RuntimeFailure({"Could not bind HTTP server port "+std::to_string(port)+".","",p.line,"HttpError"});
  if(listen(server,32)!=0) throw RuntimeFailure({"Could not listen on HTTP server port.","",p.line,"HttpError"});
  while(true){
    Socket client=accept(server,nullptr,nullptr);
    if(client==invalid_socket) continue;
    SocketGuard client_guard(client);
    std::string raw;
    char buf[8192];
    std::size_t expected=0;
    bool headers_done=false;
    while(true){
#ifdef _WIN32
      int n=recv(client,buf,sizeof(buf),0);
#else
      auto n=recv(client,buf,sizeof(buf),0);
#endif
      if(n<=0) break;
      raw.append(buf,buf+n);
      auto end=raw.find("\r\n\r\n");
      if(end!=std::string::npos&&!headers_done){
        headers_done=true;
        auto h=raw.substr(0,end);
        auto lc=lower_ascii(h);
        auto pos=lc.find("content-length:");
        if(pos!=std::string::npos){
          auto line_end=lc.find("\r\n",pos);
          auto val=trim_copy(h.substr(pos+15,line_end-(pos+15)));
          try{expected=static_cast<std::size_t>(std::stoull(val));}catch(...){expected=0;}
        }
        if(raw.size()>=end+4+expected) break;
      }else if(headers_done){
        auto end2=raw.find("\r\n\r\n");
        if(end2!=std::string::npos&&raw.size()>=end2+4+expected) break;
      }
    }
    auto end=raw.find("\r\n\r\n");
    if(end==std::string::npos) continue;
    auto head=raw.substr(0,end),body=raw.substr(end+4);
    std::istringstream in(head);
    std::string first;
    std::getline(in,first);
    if(!first.empty()&&first.back()=='\r') first.pop_back();
    std::istringstream f(first);
    std::string method,target,version;
    f>>method>>target>>version;
    std::unordered_map<std::string,std::string> headers;
    std::string line;
    while(std::getline(in,line)){
      if(!line.empty()&&line.back()=='\r') line.pop_back();
      auto c=line.find(':');
      if(c!=std::string::npos) headers[lower_ascii(trim_copy(line.substr(0,c)))]=trim_copy(line.substr(c+1));
    }
    auto r=dispatch_web(state,vm,method,target,body,headers,p);
    std::ostringstream out;
    out<<"HTTP/1.1 "<<r.status<<' '<<status_text(r.status)<<"\r\nContent-Type: "<<r.type<<"\r\nContent-Length: "<<r.body.size()<<"\r\nAccess-Control-Allow-Origin: *\r\nAccess-Control-Allow-Headers: Content-Type, Authorization\r\nAccess-Control-Allow-Methods: GET, POST, PUT, PATCH, DELETE, OPTIONS\r\nConnection: close\r\n";
    for(auto&[k,v]:r.headers) out<<k<<": "<<v<<"\r\n";
    out<<"\r\n"<<r.body;
    send_all(client,out.str());
  }
}

bool sortable_less(const Value& a, const Value& b, SourcePos p) {
  if(auto x=std::get_if<std::int64_t>(&a.data())){if(auto y=std::get_if<std::int64_t>(&b.data()))return *x<*y;}
  if(auto x=std::get_if<double>(&a.data())){if(auto y=std::get_if<double>(&b.data()))return *x<*y;}
  if(auto x=std::get_if<std::int64_t>(&a.data())){if(auto y=std::get_if<double>(&b.data()))return static_cast<double>(*x)<*y;}
  if(auto x=std::get_if<double>(&a.data())){if(auto y=std::get_if<std::int64_t>(&b.data()))return *x<static_cast<double>(*y);}
  if(auto x=std::get_if<std::string>(&a.data())){if(auto y=std::get_if<std::string>(&b.data()))return *x<*y;}
  throw Error(p,"sort needs Int, Num, or Text keys.");
}

std::int64_t normalize_slice_index(std::int64_t i, std::size_t size) {
  auto n=static_cast<std::int64_t>(size);
  if(i<0) i=n+i;
  if(i<0) i=0;
  if(i>n) i=n;
  return i;
}

std::shared_ptr<MapData> load_database(const std::string& path, SourcePos p) {
  std::ifstream in(path,std::ios::binary);
  if(!in) return std::make_shared<MapData>();
  std::string text((std::istreambuf_iterator<char>(in)),{});
  if(trim_copy(text).empty()) return std::make_shared<MapData>();
  auto value=JsonParser(text,p).parse();
  auto map=std::get_if<std::shared_ptr<MapData>>(&value.data());
  if(!map) throw RuntimeFailure({"Database file must contain a JSON object.",path,p.line,"DatabaseError"});
  return *map;
}

void save_database(const std::string& path, const std::shared_ptr<MapData>& map, SourcePos p) {
  auto parent=std::filesystem::path(path).parent_path();
  if(!parent.empty()) std::filesystem::create_directories(parent);
  std::ofstream out(path,std::ios::binary|std::ios::trunc);
  if(!out) throw RuntimeFailure({"Could not write database '"+path+"'.",path,p.line,"DatabaseError"});
  out<<json_stringify_impl(Value(map),2,0);
  if(!out) throw RuntimeFailure({"Writing database '"+path+"' failed.",path,p.line,"DatabaseError"});
}

std::atomic<std::int64_t> next_task{1};
std::mutex task_mutex;
std::unordered_map<std::int64_t,std::shared_future<std::string>> tasks;

std::int64_t start_task(std::function<std::string()> fn) {
  auto id=next_task.fetch_add(1);
  auto future=std::async(std::launch::async,std::move(fn)).share();
  std::lock_guard<std::mutex> lock(task_mutex);
  tasks.emplace(id,std::move(future));
  return id;
}

std::shared_future<std::string> task_for(std::int64_t id, SourcePos p) {
  std::lock_guard<std::mutex> lock(task_mutex);
  auto it=tasks.find(id);
  if(it==tasks.end()) throw RuntimeFailure({"Unknown async task "+std::to_string(id)+".","",p.line,"AsyncError"});
  return it->second;
}

std::shared_ptr<ModuleData> make_module(const std::string& name) {
  auto m=std::make_shared<ModuleData>();
  m->name=name;
  return m;
}

} // namespace

bool is_platform_builtin(const std::string& name) {
  static const std::set<std::string> names={
    "json","text","collections","functional","generic","option","result","match","async","database",
    "test","process","http","web","js","ts"
  };
  return names.contains(name);
}

TypeInfo platform_builtin_type(const std::string& name) {
  auto m=module_type(name);
  auto& x=m.members;
  TypeInfo unknown;
  TypeInfo text(TypeKind::Text),integer(TypeKind::Int),boolean(TypeKind::Bool),none(TypeKind::None),function(TypeKind::Function);
  auto list_unknown=list_type();
  if(name=="json"){
    x["parse"]=fn_type({text},unknown,false,0,true);
    x["stringify"]=fn_type({unknown},text);
    x["pretty"]=fn_type({unknown},text);
  }else if(name=="text"){
    x["trim"]=fn_type({text},text);x["contains"]=fn_type({text,text},boolean);x["starts"]=fn_type({text,text},boolean);x["ends"]=fn_type({text,text},boolean);
    x["replace"]=fn_type({text,text,text},text);x["split"]=fn_type({text,text},list_type(text));x["join"]=fn_type({unknown,text},text);x["repeat"]=fn_type({text,integer},text);
  }else if(name=="collections"){
    x["reverse"]=fn_type({unknown},unknown);x["contains"]=fn_type({unknown,unknown},boolean);x["first"]=fn_type({unknown},unknown);x["last"]=fn_type({unknown},unknown);
    x["unique"]=fn_type({unknown},unknown);x["sort"]=fn_type({unknown},unknown);x["keys"]=fn_type({unknown},list_type(text));x["values"]=fn_type({unknown},list_unknown);
    x["map"]=fn_type({unknown,function},list_unknown);x["filter"]=fn_type({unknown,function},list_unknown);x["reduce"]=fn_type({unknown,unknown,function},unknown);
    x["sort_by"]=fn_type({unknown,function},list_unknown);x["slice"]=fn_type({unknown,integer,integer},list_unknown);
  }else if(name=="functional"){
    x["bind1"]=fn_type({function,unknown},function);x["compose"]=fn_type({function,function},function);x["constant"]=fn_type({unknown},function);
  }else if(name=="generic"){
    x["identity"]=fn_type({unknown},unknown);x["first"]=fn_type({unknown,unknown},unknown);x["second"]=fn_type({unknown,unknown},unknown);x["pair"]=fn_type({unknown,unknown},list_unknown);
  }else if(name=="option"){
    x["some"]=fn_type({unknown},unknown);x["none"]=fn_type({},unknown);x["has"]=fn_type({unknown},boolean);x["value"]=fn_type({unknown},unknown);x["or"]=fn_type({unknown,unknown},unknown);
  }else if(name=="result"){
    x["ok"]=fn_type({unknown},unknown);x["err"]=fn_type({text},unknown);x["is_ok"]=fn_type({unknown},boolean);x["is_err"]=fn_type({unknown},boolean);x["value"]=fn_type({unknown},unknown);x["error"]=fn_type({unknown},text);
  }else if(name=="match"){
    x["value"]=fn_type({unknown,unknown},boolean);x["type"]=fn_type({unknown,text},boolean);x["when"]=fn_type({unknown,unknown,function,function},unknown);x["type_when"]=fn_type({unknown,text,function,function},unknown);
  }else if(name=="async"){
    x["run"]=fn_type({text},integer);x["http"]=fn_type({text},integer);x["await"]=fn_type({integer},text,false,0,true);x["ready"]=fn_type({integer},boolean,false,0,true);x["forget"]=fn_type({integer},none);
  }else if(name=="database"){
    x["open"]=fn_type({text},text,false,0,true);x["get"]=fn_type({text,text},unknown,false,0,true);x["set"]=fn_type({text,text,unknown},none,false,0,true);
    x["remove"]=fn_type({text,text},boolean,false,0,true);x["has"]=fn_type({text,text},boolean,false,0,true);x["keys"]=fn_type({text},list_type(text),false,0,true);
    x["all"]=fn_type({text},unknown,false,0,true);x["drop"]=fn_type({text},none,false,0,true);
  }else if(name=="test"){
    x["ok"]=fn_type({boolean},none);x["equal"]=fn_type({unknown,unknown},none);x["not_equal"]=fn_type({unknown,unknown},none);x["fail"]=fn_type({text},none);
  }else if(name=="process"){
    x["run"]=fn_type({text},integer,true,1,true);x["output"]=fn_type({text},text,false,0,true);
  }else if(name=="http"){
    x["https_supported"]=boolean;x["get"]=fn_type({text},text,false,0,true);x["post"]=fn_type({text,text},text,false,0,true);x["post_json"]=fn_type({text,text},text,false,0,true);x["request"]=fn_type({text,text,text},unknown,false,0,true);
  }else if(name=="web"){
    TypeInfo handler(TypeKind::Function);
    x["get"]=fn_type({text,handler},none);x["post"]=fn_type({text,handler},none);x["put"]=fn_type({text,handler},none);x["patch"]=fn_type({text,handler},none);x["delete"]=fn_type({text,handler},none);
    x["listen"]=fn_type({integer},none,false,0,true);x["text"]=fn_type({text},unknown);x["json"]=fn_type({text},unknown);x["response"]=fn_type({integer,text,text},unknown);
    x["method"]=fn_type({},text);x["path"]=fn_type({},text);x["query"]=fn_type({},text);x["body"]=fn_type({},text);x["header"]=fn_type({text},text);x["param"]=fn_type({text},text);
    x["handle"]=fn_type({text,text,text},text);x["handle_status"]=fn_type({text,text,text},integer);x["route_count"]=fn_type({},integer);
  }else if(name=="js"){
    x["run"]=fn_type({text},integer,false,0,true);x["output"]=fn_type({text},text,false,0,true);x["eval"]=fn_type({text},text,false,0,true);
  }else if(name=="ts"){
    x["run"]=fn_type({text},integer,false,0,true);x["compile"]=fn_type({text},integer,false,0,true);x["output"]=fn_type({text},text,false,0,true);
  }
  return m;
}

std::shared_ptr<ModuleData> platform_builtin_module(const std::string& name, Interpreter& vm) {
  auto m=make_module(name);
  if(name=="json"){
    m->exports["parse"]=callable("json.parse",1,1,[](const std::vector<Value>&a,SourcePos p){return JsonParser(require_text(a[0],p,"json.parse"),p).parse();});
    m->exports["stringify"]=callable("json.stringify",1,1,[](const std::vector<Value>&a,SourcePos){return Value(json_stringify(a[0]));});
    m->exports["pretty"]=callable("json.pretty",1,1,[](const std::vector<Value>&a,SourcePos){return Value(json_stringify_impl(a[0],2,0));});
  }else if(name=="text"){
    m->exports["trim"]=callable("text.trim",1,1,[](const std::vector<Value>&a,SourcePos p){return Value(trim_copy(require_text(a[0],p,"text.trim")));});
    m->exports["contains"]=callable("text.contains",2,2,[](const std::vector<Value>&a,SourcePos p){auto s=require_text(a[0],p,"text.contains"),q=require_text(a[1],p,"text.contains");return Value(s.find(q)!=std::string::npos);});
    m->exports["starts"]=callable("text.starts",2,2,[](const std::vector<Value>&a,SourcePos p){auto s=require_text(a[0],p,"text.starts"),q=require_text(a[1],p,"text.starts");return Value(s.rfind(q,0)==0);});
    m->exports["ends"]=callable("text.ends",2,2,[](const std::vector<Value>&a,SourcePos p){auto s=require_text(a[0],p,"text.ends"),q=require_text(a[1],p,"text.ends");return Value(s.size()>=q.size()&&s.compare(s.size()-q.size(),q.size(),q)==0);});
    m->exports["replace"]=callable("text.replace",3,3,[](const std::vector<Value>&a,SourcePos p){auto s=require_text(a[0],p,"text.replace"),from=require_text(a[1],p,"text.replace"),to=require_text(a[2],p,"text.replace");if(from.empty())return Value(s);std::size_t pos=0;while((pos=s.find(from,pos))!=std::string::npos){s.replace(pos,from.size(),to);pos+=to.size();}return Value(s);});
    m->exports["split"]=callable("text.split",2,2,[](const std::vector<Value>&a,SourcePos p){auto s=require_text(a[0],p,"text.split"),sep=require_text(a[1],p,"text.split");auto out=std::make_shared<ListData>();if(sep.empty()){for(char c:s)out->items.emplace_back(std::string(1,c));return Value(out);}std::size_t start=0;while(true){auto pos=s.find(sep,start);out->items.emplace_back(s.substr(start,pos==std::string::npos?std::string::npos:pos-start));if(pos==std::string::npos)break;start=pos+sep.size();}return Value(out);});
    m->exports["join"]=callable("text.join",2,2,[](const std::vector<Value>&a,SourcePos p){auto l=require_list(a[0],p,"text.join");auto sep=require_text(a[1],p,"text.join");std::ostringstream o;for(std::size_t i=0;i<l->items.size();++i){if(i)o<<sep;o<<require_text(l->items[i],p,"text.join");}return Value(o.str());});
    m->exports["repeat"]=callable("text.repeat",2,2,[](const std::vector<Value>&a,SourcePos p){auto s=require_text(a[0],p,"text.repeat");auto n=require_int(a[1],p,"text.repeat");if(n<0)throw Error(p,"text.repeat count cannot be negative.");std::string out;for(std::int64_t i=0;i<n;++i)out+=s;return Value(out);});
  }else if(name=="collections"){
    auto list_copy=[](const Value&v,SourcePos p,const std::string&n){auto l=require_list(v,p,n);auto out=std::make_shared<ListData>();out->items=l->items;return out;};
    m->exports["reverse"]=callable("collections.reverse",1,1,[list_copy](const std::vector<Value>&a,SourcePos p){auto out=list_copy(a[0],p,"collections.reverse");std::reverse(out->items.begin(),out->items.end());return Value(out);});
    m->exports["contains"]=callable("collections.contains",2,2,[](const std::vector<Value>&a,SourcePos p){if(auto l=std::get_if<std::shared_ptr<ListData>>(&a[0].data())){for(auto&v:(*l)->items)if(value_equal(v,a[1]))return Value(true);return Value(false);}if(auto s=std::get_if<std::shared_ptr<SetData>>(&a[0].data())){for(auto&v:(*s)->items)if(value_equal(v,a[1]))return Value(true);return Value(false);}if(auto mp=std::get_if<std::shared_ptr<MapData>>(&a[0].data())){auto key=require_text(a[1],p,"collections.contains");for(auto&[k,v]:(*mp)->items){(void)v;if(k==key)return Value(true);}return Value(false);}throw Error(p,"collections.contains needs List, Set, or Map.");});
    m->exports["first"]=callable("collections.first",1,1,[](const std::vector<Value>&a,SourcePos p){auto l=require_list(a[0],p,"collections.first");if(l->items.empty())throw Error(p,"collections.first needs a non-empty List.");return l->items.front();});
    m->exports["last"]=callable("collections.last",1,1,[](const std::vector<Value>&a,SourcePos p){auto l=require_list(a[0],p,"collections.last");if(l->items.empty())throw Error(p,"collections.last needs a non-empty List.");return l->items.back();});
    m->exports["unique"]=callable("collections.unique",1,1,[list_copy](const std::vector<Value>&a,SourcePos p){auto in=list_copy(a[0],p,"collections.unique");auto out=std::make_shared<ListData>();for(auto&v:in->items){bool seen=false;for(auto&x:out->items)if(value_equal(v,x)){seen=true;break;}if(!seen)out->items.push_back(v);}return Value(out);});
    m->exports["sort"]=callable("collections.sort",1,1,[list_copy](const std::vector<Value>&a,SourcePos p){auto out=list_copy(a[0],p,"collections.sort");std::sort(out->items.begin(),out->items.end(),[p](const Value&x,const Value&y){return sortable_less(x,y,p);});return Value(out);});
    m->exports["keys"]=callable("collections.keys",1,1,[](const std::vector<Value>&a,SourcePos p){auto mp=std::get_if<std::shared_ptr<MapData>>(&a[0].data());if(!mp)throw Error(p,"collections.keys needs a Map.");auto out=std::make_shared<ListData>();for(auto&[k,v]:(*mp)->items){(void)v;out->items.emplace_back(k);}return Value(out);});
    m->exports["values"]=callable("collections.values",1,1,[](const std::vector<Value>&a,SourcePos p){auto mp=std::get_if<std::shared_ptr<MapData>>(&a[0].data());if(!mp)throw Error(p,"collections.values needs a Map.");auto out=std::make_shared<ListData>();for(auto&[k,v]:(*mp)->items){(void)k;out->items.push_back(v);}return Value(out);});
    m->exports["map"]=callable("collections.map",2,2,[&vm](const std::vector<Value>&a,SourcePos p){auto l=require_list(a[0],p,"collections.map");auto out=std::make_shared<ListData>();for(auto&v:l->items)out->items.push_back(vm.invoke(a[1],{v},p));return Value(out);});
    m->exports["filter"]=callable("collections.filter",2,2,[&vm](const std::vector<Value>&a,SourcePos p){auto l=require_list(a[0],p,"collections.filter");auto out=std::make_shared<ListData>();for(auto&v:l->items)if(vm.invoke(a[1],{v},p).truth(p))out->items.push_back(v);return Value(out);});
    m->exports["reduce"]=callable("collections.reduce",3,3,[&vm](const std::vector<Value>&a,SourcePos p){auto l=require_list(a[0],p,"collections.reduce");Value acc=a[1];for(auto&v:l->items)acc=vm.invoke(a[2],{acc,v},p);return acc;});
    m->exports["sort_by"]=callable("collections.sort_by",2,2,[&vm](const std::vector<Value>&a,SourcePos p){auto l=require_list(a[0],p,"collections.sort_by");std::vector<std::pair<Value,Value>> keyed;keyed.reserve(l->items.size());for(auto&v:l->items)keyed.emplace_back(vm.invoke(a[1],{v},p),v);std::stable_sort(keyed.begin(),keyed.end(),[p](const auto&x,const auto&y){return sortable_less(x.first,y.first,p);});auto out=std::make_shared<ListData>();for(auto&kv:keyed)out->items.push_back(kv.second);return Value(out);});
    m->exports["slice"]=callable("collections.slice",3,3,[](const std::vector<Value>&a,SourcePos p){auto l=require_list(a[0],p,"collections.slice");auto from=normalize_slice_index(require_int(a[1],p,"collections.slice"),l->items.size());auto to=normalize_slice_index(require_int(a[2],p,"collections.slice"),l->items.size());auto out=std::make_shared<ListData>();if(to<from)return Value(out);for(auto i=from;i<to;++i)out->items.push_back(l->items[static_cast<std::size_t>(i)]);return Value(out);});
  }else if(name=="functional"){
    m->exports["bind1"]=callable("functional.bind1",2,2,[&vm](const std::vector<Value>&a,SourcePos p){auto fn=a[0],captured=a[1];if(!std::holds_alternative<std::shared_ptr<CallableData>>(fn.data()))throw Error(p,"functional.bind1 needs a function first.");return Value(callable("bound",1,1,[&vm,fn,captured](const std::vector<Value>&x,SourcePos q){return vm.invoke(fn,{captured,x[0]},q);}));});
    m->exports["compose"]=callable("functional.compose",2,2,[&vm](const std::vector<Value>&a,SourcePos p){if(!std::holds_alternative<std::shared_ptr<CallableData>>(a[0].data())||!std::holds_alternative<std::shared_ptr<CallableData>>(a[1].data()))throw Error(p,"functional.compose needs two functions.");auto outer=a[0],inner=a[1];return Value(callable("composed",1,1,[&vm,outer,inner](const std::vector<Value>&x,SourcePos q){auto middle=vm.invoke(inner,{x[0]},q);return vm.invoke(outer,{middle},q);}));});
    m->exports["constant"]=callable("functional.constant",1,1,[](const std::vector<Value>&a,SourcePos){auto value=a[0];return Value(callable("constant",1,1,[value](const std::vector<Value>&,SourcePos){return value;}));});
  }else if(name=="generic"){
    m->exports["identity"]=callable("generic.identity",1,1,[](const std::vector<Value>&a,SourcePos){return a[0];});
    m->exports["first"]=callable("generic.first",2,2,[](const std::vector<Value>&a,SourcePos){return a[0];});
    m->exports["second"]=callable("generic.second",2,2,[](const std::vector<Value>&a,SourcePos){return a[1];});
    m->exports["pair"]=callable("generic.pair",2,2,[](const std::vector<Value>&a,SourcePos){auto l=std::make_shared<ListData>();l->items={a[0],a[1]};return Value(l);});
  }else if(name=="option"){
    m->exports["some"]=callable("option.some",1,1,[](const std::vector<Value>&a,SourcePos){return Value(option_object(true,a[0]));});
    m->exports["none"]=callable("option.none",0,0,[](const std::vector<Value>&,SourcePos){return Value(option_object(false));});
    m->exports["has"]=callable("option.has",1,1,[](const std::vector<Value>&a,SourcePos p){auto o=require_tagged(a[0],"Option",p);return o->fields["has"];});
    m->exports["value"]=callable("option.value",1,1,[](const std::vector<Value>&a,SourcePos p){auto o=require_tagged(a[0],"Option",p);if(!std::get<bool>(o->fields["has"].data()))throw RuntimeFailure({"Option has no value.","",p.line,"OptionError"});return o->fields["value"];});
    m->exports["or"]=callable("option.or",2,2,[](const std::vector<Value>&a,SourcePos p){auto o=require_tagged(a[0],"Option",p);return std::get<bool>(o->fields["has"].data())?o->fields["value"]:a[1];});
  }else if(name=="result"){
    m->exports["ok"]=callable("result.ok",1,1,[](const std::vector<Value>&a,SourcePos){return Value(result_object(true,a[0],""));});
    m->exports["err"]=callable("result.err",1,1,[](const std::vector<Value>&a,SourcePos p){return Value(result_object(false,Value{},require_text(a[0],p,"result.err")));});
    m->exports["is_ok"]=callable("result.is_ok",1,1,[](const std::vector<Value>&a,SourcePos p){return require_tagged(a[0],"Result",p)->fields["ok"];});
    m->exports["is_err"]=callable("result.is_err",1,1,[](const std::vector<Value>&a,SourcePos p){auto o=require_tagged(a[0],"Result",p);return Value(!std::get<bool>(o->fields["ok"].data()));});
    m->exports["value"]=callable("result.value",1,1,[](const std::vector<Value>&a,SourcePos p){auto o=require_tagged(a[0],"Result",p);if(!std::get<bool>(o->fields["ok"].data()))throw RuntimeFailure({o->fields["error"].text(),"",p.line,"ResultError"});return o->fields["value"];});
    m->exports["error"]=callable("result.error",1,1,[](const std::vector<Value>&a,SourcePos p){return require_tagged(a[0],"Result",p)->fields["error"];});
  }else if(name=="match"){
    m->exports["value"]=callable("match.value",2,2,[](const std::vector<Value>&a,SourcePos){return Value(value_equal(a[0],a[1]));});
    m->exports["type"]=callable("match.type",2,2,[](const std::vector<Value>&a,SourcePos p){return Value(a[0].type_name()==require_text(a[1],p,"match.type"));});
    m->exports["when"]=callable("match.when",4,4,[&vm](const std::vector<Value>&a,SourcePos p){return vm.invoke(value_equal(a[0],a[1])?a[2]:a[3],{a[0]},p);});
    m->exports["type_when"]=callable("match.type_when",4,4,[&vm](const std::vector<Value>&a,SourcePos p){auto type=require_text(a[1],p,"match.type_when");return vm.invoke(a[0].type_name()==type?a[2]:a[3],{a[0]},p);});
  }else if(name=="async"){
    m->exports["run"]=callable("async.run",1,1,[](const std::vector<Value>&a,SourcePos p){auto command=require_text(a[0],p,"async.run");return Value(start_task([command,p]{return process_output(command,p);}));});
    m->exports["http"]=callable("async.http",1,1,[](const std::vector<Value>&a,SourcePos p){auto url=require_text(a[0],p,"async.http");return Value(start_task([url,p]{return http_client_request("GET",url,"","text/plain",p).body;}));});
    m->exports["await"]=callable("async.await",1,1,[](const std::vector<Value>&a,SourcePos p){return Value(task_for(require_int(a[0],p,"async.await"),p).get());});
    m->exports["ready"]=callable("async.ready",1,1,[](const std::vector<Value>&a,SourcePos p){auto f=task_for(require_int(a[0],p,"async.ready"),p);return Value(f.wait_for(std::chrono::seconds(0))==std::future_status::ready);});
    m->exports["forget"]=callable("async.forget",1,1,[](const std::vector<Value>&a,SourcePos p){auto id=require_int(a[0],p,"async.forget");std::lock_guard<std::mutex> lock(task_mutex);tasks.erase(id);return Value{};});
  }else if(name=="database"){
    m->exports["open"]=callable("database.open",1,1,[](const std::vector<Value>&a,SourcePos p){auto path=require_text(a[0],p,"database.open");if(!std::filesystem::exists(path))save_database(path,std::make_shared<MapData>(),p);else(void)load_database(path,p);return Value(path);});
    m->exports["get"]=callable("database.get",2,2,[](const std::vector<Value>&a,SourcePos p){auto path=require_text(a[0],p,"database.get"),key=require_text(a[1],p,"database.get");auto db=load_database(path,p);for(auto&[k,v]:db->items)if(k==key)return v;return Value{};});
    m->exports["set"]=callable("database.set",3,3,[](const std::vector<Value>&a,SourcePos p){auto path=require_text(a[0],p,"database.set"),key=require_text(a[1],p,"database.set");auto db=load_database(path,p);bool found=false;for(auto&[k,v]:db->items)if(k==key){v=a[2];found=true;break;}if(!found)db->items.emplace_back(key,a[2]);save_database(path,db,p);return Value{};});
    m->exports["remove"]=callable("database.remove",2,2,[](const std::vector<Value>&a,SourcePos p){auto path=require_text(a[0],p,"database.remove"),key=require_text(a[1],p,"database.remove");auto db=load_database(path,p);auto it=std::find_if(db->items.begin(),db->items.end(),[&](const auto&kv){return kv.first==key;});if(it==db->items.end())return Value(false);db->items.erase(it);save_database(path,db,p);return Value(true);});
    m->exports["has"]=callable("database.has",2,2,[](const std::vector<Value>&a,SourcePos p){auto path=require_text(a[0],p,"database.has"),key=require_text(a[1],p,"database.has");auto db=load_database(path,p);for(auto&[k,v]:db->items){(void)v;if(k==key)return Value(true);}return Value(false);});
    m->exports["keys"]=callable("database.keys",1,1,[](const std::vector<Value>&a,SourcePos p){auto db=load_database(require_text(a[0],p,"database.keys"),p);auto out=std::make_shared<ListData>();for(auto&[k,v]:db->items){(void)v;out->items.emplace_back(k);}return Value(out);});
    m->exports["all"]=callable("database.all",1,1,[](const std::vector<Value>&a,SourcePos p){return Value(load_database(require_text(a[0],p,"database.all"),p));});
    m->exports["drop"]=callable("database.drop",1,1,[](const std::vector<Value>&a,SourcePos p){auto path=require_text(a[0],p,"database.drop");std::error_code ec;std::filesystem::remove(path,ec);if(ec)throw RuntimeFailure({"Could not remove database '"+path+"'.",path,p.line,"DatabaseError"});return Value{};});
  }else if(name=="test"){
    auto fail=[](SourcePos p,const std::string&m)->Value{throw RuntimeFailure({m,"",p.line,"AssertionError"});};
    m->exports["ok"]=callable("test.ok",1,1,[fail](const std::vector<Value>&a,SourcePos p){if(!require_bool(a[0],p,"test.ok"))return fail(p,"Expected condition to be true.");return Value{};});
    m->exports["equal"]=callable("test.equal",2,2,[fail](const std::vector<Value>&a,SourcePos p){if(!value_equal(a[0],a[1]))return fail(p,"Expected "+a[0].text()+" to equal "+a[1].text()+".");return Value{};});
    m->exports["not_equal"]=callable("test.not_equal",2,2,[fail](const std::vector<Value>&a,SourcePos p){if(value_equal(a[0],a[1]))return fail(p,"Expected values to be different.");return Value{};});
    m->exports["fail"]=callable("test.fail",1,1,[fail](const std::vector<Value>&a,SourcePos p){return fail(p,require_text(a[0],p,"test.fail"));});
  }else if(name=="process"){
    m->exports["run"]=callable("process.run",1,1,[](const std::vector<Value>&a,SourcePos p){return Value(static_cast<std::int64_t>(normalized_system(require_text(a[0],p,"process.run"))));});
    m->exports["output"]=callable("process.output",1,1,[](const std::vector<Value>&a,SourcePos p){return Value(process_output(require_text(a[0],p,"process.output"),p));});
  }else if(name=="http"){
    m->exports["https_supported"]=Value(true);
    m->exports["get"]=callable("http.get",1,1,[](const std::vector<Value>&a,SourcePos p){return Value(http_client_request("GET",require_text(a[0],p,"http.get"),"","text/plain",p).body);});
    m->exports["post"]=callable("http.post",2,2,[](const std::vector<Value>&a,SourcePos p){return Value(http_client_request("POST",require_text(a[0],p,"http.post"),require_text(a[1],p,"http.post"),"text/plain; charset=utf-8",p).body);});
    m->exports["post_json"]=callable("http.post_json",2,2,[](const std::vector<Value>&a,SourcePos p){return Value(http_client_request("POST",require_text(a[0],p,"http.post_json"),require_text(a[1],p,"http.post_json"),"application/json",p).body);});
    m->exports["request"]=callable("http.request",3,3,[](const std::vector<Value>&a,SourcePos p){auto r=http_client_request(require_text(a[0],p,"http.request"),require_text(a[1],p,"http.request"),require_text(a[2],p,"http.request"),"text/plain; charset=utf-8",p);return Value(response_object(r.status,r.body,r.type));});
  }else if(name=="web"){
    auto state=std::make_shared<WebState>();
    auto route=[state](std::string method){return callable("web."+lower_ascii(method),2,2,[state,method](const std::vector<Value>&a,SourcePos p){auto path=require_text(a[0],p,"web route");if(!std::holds_alternative<std::shared_ptr<CallableData>>(a[1].data()))throw Error(p,"web route needs a function handler as its second value.");state->routes.push_back({method,path,a[1]});return Value{};});};
    m->exports["get"]=route("GET");m->exports["post"]=route("POST");m->exports["put"]=route("PUT");m->exports["patch"]=route("PATCH");m->exports["delete"]=route("DELETE");
    m->exports["text"]=callable("web.text",1,1,[](const std::vector<Value>&a,SourcePos p){return Value(response_object(200,require_text(a[0],p,"web.text"),"text/plain; charset=utf-8"));});
    m->exports["json"]=callable("web.json",1,1,[](const std::vector<Value>&a,SourcePos p){return Value(response_object(200,require_text(a[0],p,"web.json"),"application/json; charset=utf-8"));});
    m->exports["response"]=callable("web.response",3,3,[](const std::vector<Value>&a,SourcePos p){return Value(response_object(require_int(a[0],p,"web.response"),require_text(a[1],p,"web.response"),require_text(a[2],p,"web.response")));});
    m->exports["method"]=callable("web.method",0,0,[state](const std::vector<Value>&,SourcePos){return Value(state->current.method);});
    m->exports["path"]=callable("web.path",0,0,[state](const std::vector<Value>&,SourcePos){return Value(state->current.path);});
    m->exports["query"]=callable("web.query",0,0,[state](const std::vector<Value>&,SourcePos){return Value(state->current.query);});
    m->exports["body"]=callable("web.body",0,0,[state](const std::vector<Value>&,SourcePos){return Value(state->current.body);});
    m->exports["header"]=callable("web.header",1,1,[state](const std::vector<Value>&a,SourcePos p){auto k=lower_ascii(require_text(a[0],p,"web.header"));auto i=state->current.headers.find(k);return Value(i==state->current.headers.end()?std::string{}:i->second);});
    m->exports["param"]=callable("web.param",1,1,[state](const std::vector<Value>&a,SourcePos p){auto k=require_text(a[0],p,"web.param");auto i=state->current.params.find(k);return Value(i==state->current.params.end()?std::string{}:i->second);});
    m->exports["handle"]=callable("web.handle",3,3,[state,&vm](const std::vector<Value>&a,SourcePos p){auto r=dispatch_web(*state,vm,require_text(a[0],p,"web.handle"),require_text(a[1],p,"web.handle"),require_text(a[2],p,"web.handle"),{},p);return Value(r.body);});
    m->exports["handle_status"]=callable("web.handle_status",3,3,[state,&vm](const std::vector<Value>&a,SourcePos p){auto r=dispatch_web(*state,vm,require_text(a[0],p,"web.handle_status"),require_text(a[1],p,"web.handle_status"),require_text(a[2],p,"web.handle_status"),{},p);return Value(static_cast<std::int64_t>(r.status));});
    m->exports["route_count"]=callable("web.route_count",0,0,[state](const std::vector<Value>&,SourcePos){return Value(static_cast<std::int64_t>(state->routes.size()));});
    m->exports["listen"]=callable("web.listen",1,1,[state,&vm](const std::vector<Value>&a,SourcePos p){auto port=require_int(a[0],p,"web.listen");if(port<1||port>65535)throw Error(p,"web.listen port must be 1..65535.");serve_web(*state,vm,static_cast<int>(port),p);return Value{};});
  }else if(name=="js"){
    m->exports["run"]=callable("js.run",1,1,[](const std::vector<Value>&a,SourcePos p){auto file=require_text(a[0],p,"js.run");return Value(static_cast<std::int64_t>(normalized_system("node "+shell_quote(file))));});
    m->exports["output"]=callable("js.output",1,1,[](const std::vector<Value>&a,SourcePos p){return Value(process_output("node "+shell_quote(require_text(a[0],p,"js.output")),p));});
    m->exports["eval"]=callable("js.eval",1,1,[](const std::vector<Value>&a,SourcePos p){return Value(process_output("node -e "+shell_quote(require_text(a[0],p,"js.eval")),p));});
  }else if(name=="ts"){
    m->exports["run"]=callable("ts.run",1,1,[](const std::vector<Value>&a,SourcePos p){return Value(static_cast<std::int64_t>(normalized_system("ts-node "+shell_quote(require_text(a[0],p,"ts.run")))));});
    m->exports["compile"]=callable("ts.compile",1,1,[](const std::vector<Value>&a,SourcePos p){return Value(static_cast<std::int64_t>(normalized_system("tsc --pretty false "+shell_quote(require_text(a[0],p,"ts.compile")))));});
    m->exports["output"]=callable("ts.output",1,1,[](const std::vector<Value>&a,SourcePos p){return Value(process_output("ts-node "+shell_quote(require_text(a[0],p,"ts.output")),p));});
  }
  return m;
}

} // namespace s
