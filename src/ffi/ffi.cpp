#include "s/ffi.hpp"
#include <cstdlib>
#include <filesystem>
#include <vector>
#ifndef _WIN32
#include <dlfcn.h>
#endif

namespace s {
namespace {
[[noreturn]] void native_fail(SourcePos p,const std::string&m){throw RuntimeFailure({m,"",p.line,"NativeError"});}
std::string handle_tag(const std::string&t){return t.rfind("Handle:",0)==0?t.substr(7):std::string{};}
#ifndef _WIN32
std::shared_ptr<void> open_library(const ast::Module&m){
  std::filesystem::path meta=m.path;std::vector<std::filesystem::path> dirs{meta.parent_path(),std::filesystem::current_path()};
  if(const char*path=std::getenv("S_NATIVE_PATH")){std::string s=path;std::size_t a=0;while(a<=s.size()){auto b=s.find(':',a);dirs.emplace_back(s.substr(a,b==std::string::npos?std::string::npos:b-a));if(b==std::string::npos)break;a=b+1;}}
#ifdef S_NATIVE_BUILD_DIR
  dirs.emplace_back(S_NATIVE_BUILD_DIR);
#endif
  std::vector<std::string> names{m.native_library,"lib"+m.native_library+".so","lib"+m.native_library+".dylib",m.native_library+".so",m.native_library+".dylib"};
  for(auto&d:dirs)for(auto&n:names){auto p=d/n;if(auto*h=dlopen(p.string().c_str(),RTLD_NOW|RTLD_LOCAL))return std::shared_ptr<void>(h,[](void*x){dlclose(x);});}
  if(auto*h=dlopen(m.native_library.c_str(),RTLD_NOW|RTLD_LOCAL))return std::shared_ptr<void>(h,[](void*x){dlclose(x);});
  throw RuntimeFailure({"Could not load native library '"+m.native_library+"'.","",1,"NativeError"});
}
void* symbol(const std::shared_ptr<void>&lib,const std::string&name,SourcePos p){auto*x=dlsym(lib.get(),name.c_str());if(!x)native_fail(p,"Native symbol '"+name+"' was not found.");return x;}
#endif

Value call_native(const std::shared_ptr<void>&lib,const ast::NativeFunction&f,const std::vector<Value>&a,SourcePos p){
#ifdef _WIN32
  (void)lib;(void)f;(void)a;native_fail(p,"Native modules are not enabled on Windows yet.");
#else
  auto raw=symbol(lib,f.symbol,p);auto all=[&](const char*t){for(auto&x:f.args)if(x!=t)return false;return true;};
  if(a.size()!=f.args.size())native_fail(p,"Native function '"+f.name+"' got the wrong number of values.");
  auto finish_int=[&](int r)->Value{if(f.fallible&&r<0)native_fail(p,"Native function '"+f.name+"' reported an error.");if(f.result=="Bool")return r!=0;if(f.result=="None")return {};return static_cast<std::int64_t>(r);};
  if(all("Int")){
    std::vector<int> v;for(auto&x:a){if(!std::holds_alternative<std::int64_t>(x.data()))native_fail(p,"Native Int argument has the wrong type.");v.push_back(static_cast<int>(std::get<std::int64_t>(x.data())));}
    if(f.result=="Int"||f.result=="Bool"||f.result=="None"){
      int r=0;if(v.empty())r=reinterpret_cast<int(*)()>(raw)();else if(v.size()==1)r=reinterpret_cast<int(*)(int)>(raw)(v[0]);else if(v.size()==2)r=reinterpret_cast<int(*)(int,int)>(raw)(v[0],v[1]);else if(v.size()==3)r=reinterpret_cast<int(*)(int,int,int)>(raw)(v[0],v[1],v[2]);else native_fail(p,"S 0.2 native Int calls support up to 3 values.");return finish_int(r);
    }
  }
  if(all("Num")&&f.result=="Num"){
    std::vector<double> v;for(auto&x:a){if(auto n=std::get_if<double>(&x.data()))v.push_back(*n);else if(auto n=std::get_if<std::int64_t>(&x.data()))v.push_back(static_cast<double>(*n));else native_fail(p,"Native Num argument has the wrong type.");}
    double r=0;if(v.empty())r=reinterpret_cast<double(*)()>(raw)();else if(v.size()==1)r=reinterpret_cast<double(*)(double)>(raw)(v[0]);else if(v.size()==2)r=reinterpret_cast<double(*)(double,double)>(raw)(v[0],v[1]);else if(v.size()==3)r=reinterpret_cast<double(*)(double,double,double)>(raw)(v[0],v[1],v[2]);else native_fail(p,"S 0.2 native Num calls support up to 3 values.");return r;
  }
  if(all("Bool")&&(f.result=="Bool"||f.result=="Int")){
    std::vector<int> v;for(auto&x:a){if(!std::holds_alternative<bool>(x.data()))native_fail(p,"Native Bool argument has the wrong type.");v.push_back(std::get<bool>(x.data())?1:0);}int r=0;if(v.empty())r=reinterpret_cast<int(*)()>(raw)();else if(v.size()==1)r=reinterpret_cast<int(*)(int)>(raw)(v[0]);else if(v.size()==2)r=reinterpret_cast<int(*)(int,int)>(raw)(v[0],v[1]);else native_fail(p,"S 0.2 native Bool calls support up to 2 values.");return f.result=="Bool"?Value(r!=0):Value(static_cast<std::int64_t>(r));
  }
  if(all("Text")&&f.result=="Text"){
    std::vector<const char*> v;std::vector<std::string> keep;for(auto&x:a){if(!std::holds_alternative<std::string>(x.data()))native_fail(p,"Native Text argument has the wrong type.");keep.push_back(std::get<std::string>(x.data()));}for(auto&x:keep)v.push_back(x.c_str());const char*r=nullptr;if(v.empty())r=reinterpret_cast<const char*(*)()>(raw)();else if(v.size()==1)r=reinterpret_cast<const char*(*)(const char*)>(raw)(v[0]);else if(v.size()==2)r=reinterpret_cast<const char*(*)(const char*,const char*)>(raw)(v[0],v[1]);else native_fail(p,"S 0.2 native Text calls support up to 2 values.");if(!r)native_fail(p,"Native function '"+f.name+"' returned no Text.");return std::string(r);
  }
  if(f.args.empty()&&f.result.rfind("Handle:",0)==0){
    void*ptr=reinterpret_cast<void*(*)()>(raw)();if(!ptr)native_fail(p,"Native resource creation failed.");auto h=std::make_shared<NativeHandleData>();h->tag=handle_tag(f.result);if(f.cleanup.empty())h->resource=std::shared_ptr<void>(ptr,[](void*){});else{auto destroy=reinterpret_cast<void(*)(void*)>(symbol(lib,f.cleanup,p));auto keep=lib;h->resource=std::shared_ptr<void>(ptr,[destroy,keep](void*x){destroy(x);});}return h;
  }
  if(f.args.size()==1&&f.args[0].rfind("Handle:",0)==0){auto h=std::get_if<std::shared_ptr<NativeHandleData>>(&a[0].data());if(!h||(*h)->tag!=handle_tag(f.args[0]))native_fail(p,"Native handle has the wrong type.");void*ptr=(*h)->resource.get();if(f.result=="None"){reinterpret_cast<void(*)(void*)>(raw)(ptr);return {};}if(f.result=="Int")return static_cast<std::int64_t>(reinterpret_cast<int(*)(void*)>(raw)(ptr));}
  native_fail(p,"This native signature needs a generated C wrapper in S 0.2.");
#endif
}
}

std::shared_ptr<ModuleData> load_native_module(const ast::Module&module){
  auto out=std::make_shared<ModuleData>();out->name=module.name;
#ifndef _WIN32
  auto lib=open_library(module);
  for(auto f:module.native_functions){auto c=std::make_shared<CallableData>();c->name=f.name;c->min_args=f.args.size();c->max_args=f.args.size();c->call=[lib,f](const std::vector<Value>&a,SourcePos p){return call_native(lib,f,a,p);};out->exports[f.name]=c;}
#else
  (void)module;
#endif
  return out;
}

} // namespace s
