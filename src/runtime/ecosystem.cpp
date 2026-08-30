#include "s/ecosystem.hpp"
#include "s/error.hpp"
#include "s/interpreter.hpp"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <limits>
#include <map>
#include <numeric>
#include <set>
#include <sstream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#ifndef _WIN32
#include <sys/wait.h>
#endif

namespace s {
namespace {

std::shared_ptr<CallableData> callable(std::string name,std::size_t min,std::size_t max,
    std::function<Value(const std::vector<Value>&,SourcePos)> fn,bool variadic=false){
  auto c=std::make_shared<CallableData>();c->name=std::move(name);c->min_args=min;c->max_args=max;c->variadic=variadic;c->call=std::move(fn);return c;
}
std::shared_ptr<FunctionSig> sig(std::vector<TypeInfo> params,TypeInfo result,bool variadic=false,std::size_t min=0,bool fallible=false){
  auto s=std::make_shared<FunctionSig>();s->params=std::move(params);s->result=std::move(result);s->variadic=variadic;s->min_args=min;s->fallible=fallible;return s;
}
TypeInfo fn(std::vector<TypeInfo> params,TypeInfo result,bool variadic=false,std::size_t min=0,bool fallible=false){TypeInfo t(TypeKind::Function);t.callable=sig(std::move(params),std::move(result),variadic,min,fallible);return t;}
TypeInfo module_type(const std::string& name){TypeInfo t(TypeKind::Module);t.name=name;return t;}
TypeInfo list_type(TypeInfo element=TypeInfo{}){TypeInfo t(TypeKind::List);t.element=std::make_shared<TypeInfo>(std::move(element));return t;}
TypeInfo map_type(){TypeInfo t(TypeKind::Map);t.key=std::make_shared<TypeInfo>(TypeInfo(TypeKind::Text));t.value=std::make_shared<TypeInfo>(TypeInfo{});return t;}
std::shared_ptr<ModuleData> make_module(const std::string& name){auto m=std::make_shared<ModuleData>();m->name=name;return m;}

double number(const Value& v,SourcePos p,const std::string& name){if(auto x=std::get_if<std::int64_t>(&v.data()))return static_cast<double>(*x);if(auto x=std::get_if<double>(&v.data()))return *x;throw Error(p,name+" needs a number.");}
std::int64_t integer(const Value& v,SourcePos p,const std::string& name){if(auto x=std::get_if<std::int64_t>(&v.data()))return *x;throw Error(p,name+" needs Int.");}
std::string text(const Value& v,SourcePos p,const std::string& name){if(auto x=std::get_if<std::string>(&v.data()))return *x;throw Error(p,name+" needs Text.");}
bool boolean(const Value& v,SourcePos p,const std::string& name){if(auto x=std::get_if<bool>(&v.data()))return *x;throw Error(p,name+" needs Bool.");}
std::shared_ptr<ListData> list(const Value& v,SourcePos p,const std::string& name){if(auto x=std::get_if<std::shared_ptr<ListData>>(&v.data()))return *x;throw Error(p,name+" needs a List.");}
std::shared_ptr<MapData> map_value(const Value& v,SourcePos p,const std::string& name){if(auto x=std::get_if<std::shared_ptr<MapData>>(&v.data()))return *x;throw Error(p,name+" needs a Map.");}

std::string shell_quote(const std::string& s){
#ifdef _WIN32
  std::string out="\"";for(char c:s){if(c=='\"')out+="\\\"";else out+=c;}return out+"\"";
#else
  std::string out="'";for(char c:s){if(c=='\'')out+="'\\''";else out+=c;}return out+"'";
#endif
}
int normalized_system(const std::string& command){int code=std::system(command.c_str());
#ifndef _WIN32
  if(code!=-1&&WIFEXITED(code))return WEXITSTATUS(code);
#endif
  return code;
}
std::string process_output(const std::string& command,SourcePos p){
#ifdef _WIN32
  FILE* pipe=_popen(command.c_str(),"r");
#else
  FILE* pipe=popen(command.c_str(),"r");
#endif
  if(!pipe)throw Error(p,"Could not start process.");std::string out;char buf[4096];while(std::fgets(buf,sizeof(buf),pipe))out+=buf;
#ifdef _WIN32
  _pclose(pipe);
#else
  pclose(pipe);
#endif
  return out;
}

struct NetResponse{std::int64_t status=0;std::string type;std::string body;};
NetResponse curl_request(const std::string& method,const std::string& url,const std::string& body,const std::string& content_type,SourcePos p){
  if(url.rfind("http://",0)!=0&&url.rfind("https://",0)!=0)throw Error(p,"net needs an http:// or https:// URL.");
  std::string marker="__SE_NET_META_7C88__";
  std::string cmd="curl -sS -L --max-time 30 -X "+shell_quote(method)+" -H "+shell_quote("Content-Type: "+content_type);
  if(!body.empty()||method=="POST"||method=="PUT"||method=="PATCH")cmd+=" --data-binary "+shell_quote(body);
  cmd+=" -w "+shell_quote("\\n"+marker+"%{http_code}|%{content_type}")+" "+shell_quote(url)+" 2>&1";
  auto out=process_output(cmd,p);auto pos=out.rfind("\n"+marker);if(pos==std::string::npos)throw Error(p,"Network request failed. Make sure curl is installed and the URL is reachable.");
  NetResponse r;r.body=out.substr(0,pos);auto meta=out.substr(pos+1+marker.size());auto bar=meta.find('|');try{r.status=std::stoll(meta.substr(0,bar));}catch(...){r.status=0;}if(bar!=std::string::npos)r.type=meta.substr(bar+1);while(!r.type.empty()&&(r.type.back()=='\n'||r.type.back()=='\r'))r.type.pop_back();return r;
}
Value response_map(const NetResponse& r){auto m=std::make_shared<MapData>();m->items.emplace_back("status",Value(r.status));m->items.emplace_back("type",Value(r.type));m->items.emplace_back("body",Value(r.body));return Value(m);}

std::vector<double> numeric_list(const Value& value,SourcePos p,const std::string& name){auto l=list(value,p,name);if(l->items.empty())throw Error(p,name+" needs a non-empty List.");std::vector<double> out;out.reserve(l->items.size());for(auto& v:l->items)out.push_back(number(v,p,name));return out;}

std::int64_t factorial_checked(std::int64_t n,SourcePos p,const std::string& name){if(n<0)throw Error(p,name+" needs a non-negative Int.");if(n>20)throw Error(p,name+" is limited to 20 for Int safety.");std::int64_t r=1;for(std::int64_t i=2;i<=n;++i)r*=i;return r;}

std::string html_escape(const std::string& s){std::string o;for(char c:s){switch(c){case '&':o+="&amp;";break;case '<':o+="&lt;";break;case '>':o+="&gt;";break;case '"':o+="&quot;";break;default:o+=c;}}return o;}
std::string js_escape(const std::string& s){std::string o;for(char c:s){switch(c){case '\\':o+="\\\\";break;case '"':o+="\\\"";break;case '\n':o+="\\n";break;case '\r':o+="\\r";break;default:o+=c;}}return o;}
struct GameScene{int width=800,height=600;std::string title="SE Game";std::string background="#111";std::vector<std::string> draw;std::vector<std::string> scripts;};
std::unordered_map<std::int64_t,GameScene>& scenes(){static std::unordered_map<std::int64_t,GameScene> value;return value;}
std::int64_t& next_scene(){static std::int64_t value=1;return value;}
GameScene& scene_for(std::int64_t id,SourcePos p){auto i=scenes().find(id);if(i==scenes().end())throw Error(p,"Unknown game scene "+std::to_string(id)+".");return i->second;}
std::string scene_html(std::int64_t id,SourcePos p){auto& s=scene_for(id,p);std::ostringstream o;o<<"<!doctype html><html><head><meta charset=\"utf-8\"><meta name=\"viewport\" content=\"width=device-width,initial-scale=1\"><title>"<<html_escape(s.title)<<"</title><style>html,body{margin:0;width:100%;height:100%;background:#000;display:grid;place-items:center;overflow:hidden}canvas{max-width:100vw;max-height:100vh;image-rendering:auto}</style></head><body><canvas id=\"game\" width=\""<<s.width<<"\" height=\""<<s.height<<"\"></canvas><script>const canvas=document.getElementById('game');const ctx=canvas.getContext('2d');const SEGame={keys:new Set(),mouse:{x:0,y:0,down:false},canvas,ctx};addEventListener('keydown',e=>SEGame.keys.add(e.key));addEventListener('keyup',e=>SEGame.keys.delete(e.key));canvas.addEventListener('mousemove',e=>{const r=canvas.getBoundingClientRect();SEGame.mouse.x=(e.clientX-r.left)*canvas.width/r.width;SEGame.mouse.y=(e.clientY-r.top)*canvas.height/r.height});canvas.addEventListener('mousedown',()=>SEGame.mouse.down=true);addEventListener('mouseup',()=>SEGame.mouse.down=false);ctx.fillStyle=\""<<js_escape(s.background)<<"\";ctx.fillRect(0,0,canvas.width,canvas.height);";for(auto& d:s.draw)o<<d;for(auto& x:s.scripts)o<<x;o<<"</script></body></html>";return o.str();}
void open_file(const std::filesystem::path& path){
#ifdef _WIN32
  normalized_system("cmd /c start \"\" "+shell_quote(path.string()));
#elif __APPLE__
  normalized_system("open "+shell_quote(path.string()));
#else
  normalized_system("xdg-open "+shell_quote(path.string())+" >/dev/null 2>&1 &");
#endif
}

} // namespace

bool is_ecosystem_builtin(const std::string& name){static const std::set<std::string> names={"math","data","net","node","next","game"};return names.contains(name);}

TypeInfo ecosystem_builtin_type(const std::string& name){
  auto m=module_type(name);auto& x=m.members;TypeInfo unknown,none(TypeKind::None),num(TypeKind::Num),integer_t(TypeKind::Int),text_t(TypeKind::Text),bool_t(TypeKind::Bool),list_t=list_type(),map_t=map_type();
  if(name=="math"){
    x["pi"]=num;x["e"]=num;x["tau"]=num;x["inf"]=num;
    for(auto n:{"sqrt","cbrt","abs","floor","ceil","round","trunc","sin","cos","tan","asin","acos","atan","sinh","cosh","tanh","asinh","acosh","atanh","exp","exp2","expm1","log","log10","log2","log1p","degrees","radians","gamma","lgamma","erf","erfc"})x[n]=fn({num},num);
    x["atan2"]=fn({num,num},num);x["pow"]=fn({num,num},num);x["hypot"]=fn({num,num},num,true,2);x["fmod"]=fn({num,num},num);x["remainder"]=fn({num,num},num);x["copysign"]=fn({num,num},num);x["nextafter"]=fn({num,num},num);x["min"]=fn({num,num},num,true,2);x["max"]=fn({num,num},num,true,2);x["clamp"]=fn({num,num,num},num);x["lerp"]=fn({num,num,num},num);x["map_range"]=fn({num,num,num,num,num},num);x["sign"]=fn({num},integer_t);x["isfinite"]=fn({num},bool_t);x["isinf"]=fn({num},bool_t);x["isnan"]=fn({num},bool_t);
    x["gcd"]=fn({integer_t,integer_t},integer_t,true,2);x["lcm"]=fn({integer_t,integer_t},integer_t,true,2);x["factorial"]=fn({integer_t},integer_t);x["comb"]=fn({integer_t,integer_t},integer_t);x["perm"]=fn({integer_t,integer_t},integer_t);
    x["sum"]=fn({list_t},num);x["mean"]=fn({list_t},num);x["median"]=fn({list_t},num);x["variance"]=fn({list_t},num);x["stddev"]=fn({list_t},num);
  }else if(name=="data"){
    x["append"]=fn({list_t,unknown},none);x["extend"]=fn({list_t,list_t},none);x["insert"]=fn({list_t,integer_t,unknown},none);x["pop"]=fn({list_t},unknown,true,1);x["clear"]=fn({unknown},none);x["copy"]=fn({unknown},unknown);x["get"]=fn({map_t,text_t},unknown,true,2);x["set"]=fn({map_t,text_t,unknown},none);x["update"]=fn({map_t,map_t},none);x["delete"]=fn({map_t,text_t},bool_t);x["has"]=fn({unknown,unknown},bool_t);x["keys"]=fn({map_t},list_type(text_t));x["values"]=fn({map_t},list_t);x["items"]=fn({map_t},list_t);
  }else if(name=="net"){
    x["get"]=fn({text_t},text_t,false,0,true);x["post"]=fn({text_t,text_t},text_t,false,0,true);x["post_json"]=fn({text_t,text_t},text_t,false,0,true);x["request"]=fn({text_t,text_t,text_t},unknown,false,0,true);x["download"]=fn({text_t,text_t},none,false,0,true);
  }else if(name=="node"){
    x["version"]=fn({},text_t,false,0,true);x["run"]=fn({text_t},integer_t,false,0,true);x["output"]=fn({text_t},text_t,false,0,true);x["eval"]=fn({text_t},text_t,false,0,true);x["npm"]=fn({text_t},integer_t,false,0,true);x["npx"]=fn({text_t},integer_t,false,0,true);
  }else if(name=="next"){
    x["create"]=fn({text_t},integer_t,false,0,true);x["dev"]=fn({text_t},integer_t,false,0,true);x["build"]=fn({text_t},integer_t,false,0,true);x["start"]=fn({text_t},integer_t,false,0,true);x["lint"]=fn({text_t},integer_t,false,0,true);
  }else if(name=="game"){
    x["new"]=fn({integer_t,integer_t,text_t},integer_t);x["background"]=fn({integer_t,text_t},none);x["clear"]=fn({integer_t},none);x["rect"]=fn({integer_t,num,num,num,num,text_t,bool_t},none);x["circle"]=fn({integer_t,num,num,num,text_t,bool_t},none);x["line"]=fn({integer_t,num,num,num,num,text_t,num},none);x["text"]=fn({integer_t,text_t,num,num,num,text_t},none);x["script"]=fn({integer_t,text_t},none);x["html"]=fn({integer_t},text_t);x["save"]=fn({integer_t,text_t},none,false,0,true);x["show"]=fn({integer_t},none,false,0,true);
  }
  return m;
}

std::shared_ptr<ModuleData> ecosystem_builtin_module(const std::string& name,Interpreter& vm){
  (void)vm;auto m=make_module(name);
  if(name=="math"){
    m->exports["pi"]=Value(3.141592653589793238462643383279502884);m->exports["e"]=Value(2.718281828459045235360287471352662498);m->exports["tau"]=Value(6.283185307179586476925286766559005768);m->exports["inf"]=Value(std::numeric_limits<double>::infinity());
#define SE_MATH1(NAME,FN) m->exports[#NAME]=callable("math." #NAME,1,1,[](const std::vector<Value>&a,SourcePos p){return Value(FN(number(a[0],p,"math." #NAME)));})
    SE_MATH1(cbrt,std::cbrt);SE_MATH1(abs,std::fabs);SE_MATH1(floor,std::floor);SE_MATH1(ceil,std::ceil);SE_MATH1(round,std::round);SE_MATH1(trunc,std::trunc);SE_MATH1(sin,std::sin);SE_MATH1(cos,std::cos);SE_MATH1(tan,std::tan);SE_MATH1(asin,std::asin);SE_MATH1(acos,std::acos);SE_MATH1(atan,std::atan);SE_MATH1(sinh,std::sinh);SE_MATH1(cosh,std::cosh);SE_MATH1(tanh,std::tanh);SE_MATH1(asinh,std::asinh);SE_MATH1(acosh,std::acosh);SE_MATH1(atanh,std::atanh);SE_MATH1(exp,std::exp);SE_MATH1(exp2,std::exp2);SE_MATH1(expm1,std::expm1);SE_MATH1(log,std::log);SE_MATH1(log10,std::log10);SE_MATH1(log2,std::log2);SE_MATH1(log1p,std::log1p);SE_MATH1(gamma,std::tgamma);SE_MATH1(lgamma,std::lgamma);SE_MATH1(erf,std::erf);SE_MATH1(erfc,std::erfc);
#undef SE_MATH1
    m->exports["sqrt"]=callable("math.sqrt",1,1,[](const std::vector<Value>&a,SourcePos p){auto x=number(a[0],p,"math.sqrt");if(x<0)throw Error(p,"math.sqrt needs a non-negative number.");return Value(std::sqrt(x));});
    m->exports["degrees"]=callable("math.degrees",1,1,[](const std::vector<Value>&a,SourcePos p){return Value(number(a[0],p,"math.degrees")*180.0/3.14159265358979323846);});m->exports["radians"]=callable("math.radians",1,1,[](const std::vector<Value>&a,SourcePos p){return Value(number(a[0],p,"math.radians")*3.14159265358979323846/180.0);});
    auto binary_math=[&](const std::string& n,auto f){m->exports[n]=callable("math."+n,2,2,[n,f](const std::vector<Value>&a,SourcePos p){return Value(f(number(a[0],p,"math."+n),number(a[1],p,"math."+n)));});};
    binary_math("pow",[](double a,double b){return std::pow(a,b);});binary_math("atan2",[](double a,double b){return std::atan2(a,b);});binary_math("fmod",[](double a,double b){return std::fmod(a,b);});binary_math("remainder",[](double a,double b){return std::remainder(a,b);});binary_math("copysign",[](double a,double b){return std::copysign(a,b);});binary_math("nextafter",[](double a,double b){return std::nextafter(a,b);});
    m->exports["hypot"]=callable("math.hypot",2,64,[](const std::vector<Value>&a,SourcePos p){double sum=0;for(auto& v:a){auto x=number(v,p,"math.hypot");sum+=x*x;}return Value(std::sqrt(sum));},true);
    m->exports["min"]=callable("math.min",2,64,[](const std::vector<Value>&a,SourcePos p){double r=number(a[0],p,"math.min");for(std::size_t i=1;i<a.size();++i)r=std::min(r,number(a[i],p,"math.min"));return Value(r);},true);m->exports["max"]=callable("math.max",2,64,[](const std::vector<Value>&a,SourcePos p){double r=number(a[0],p,"math.max");for(std::size_t i=1;i<a.size();++i)r=std::max(r,number(a[i],p,"math.max"));return Value(r);},true);
    m->exports["clamp"]=callable("math.clamp",3,3,[](const std::vector<Value>&a,SourcePos p){auto x=number(a[0],p,"math.clamp"),lo=number(a[1],p,"math.clamp"),hi=number(a[2],p,"math.clamp");if(lo>hi)throw Error(p,"math.clamp needs min <= max.");return Value(std::clamp(x,lo,hi));});m->exports["lerp"]=callable("math.lerp",3,3,[](const std::vector<Value>&a,SourcePos p){auto x=number(a[0],p,"math.lerp"),y=number(a[1],p,"math.lerp"),t=number(a[2],p,"math.lerp");return Value(x+(y-x)*t);});m->exports["map_range"]=callable("math.map_range",5,5,[](const std::vector<Value>&a,SourcePos p){auto x=number(a[0],p,"math.map_range"),a0=number(a[1],p,"math.map_range"),a1=number(a[2],p,"math.map_range"),b0=number(a[3],p,"math.map_range"),b1=number(a[4],p,"math.map_range");if(a0==a1)throw Error(p,"math.map_range input range cannot have zero width.");return Value(b0+(x-a0)*(b1-b0)/(a1-a0));});
    m->exports["sign"]=callable("math.sign",1,1,[](const std::vector<Value>&a,SourcePos p){auto x=number(a[0],p,"math.sign");return Value(static_cast<std::int64_t>((x>0)-(x<0)));});m->exports["isfinite"]=callable("math.isfinite",1,1,[](const std::vector<Value>&a,SourcePos p){return Value(std::isfinite(number(a[0],p,"math.isfinite")));});m->exports["isinf"]=callable("math.isinf",1,1,[](const std::vector<Value>&a,SourcePos p){return Value(std::isinf(number(a[0],p,"math.isinf")));});m->exports["isnan"]=callable("math.isnan",1,1,[](const std::vector<Value>&a,SourcePos p){return Value(std::isnan(number(a[0],p,"math.isnan")));});
    m->exports["gcd"]=callable("math.gcd",2,64,[](const std::vector<Value>&a,SourcePos p){auto r=integer(a[0],p,"math.gcd");for(std::size_t i=1;i<a.size();++i)r=std::gcd(r,integer(a[i],p,"math.gcd"));return Value(r);},true);m->exports["lcm"]=callable("math.lcm",2,64,[](const std::vector<Value>&a,SourcePos p){auto r=integer(a[0],p,"math.lcm");for(std::size_t i=1;i<a.size();++i)r=std::lcm(r,integer(a[i],p,"math.lcm"));return Value(r);},true);
    m->exports["factorial"]=callable("math.factorial",1,1,[](const std::vector<Value>&a,SourcePos p){return Value(factorial_checked(integer(a[0],p,"math.factorial"),p,"math.factorial"));});m->exports["comb"]=callable("math.comb",2,2,[](const std::vector<Value>&a,SourcePos p){auto n=integer(a[0],p,"math.comb"),k=integer(a[1],p,"math.comb");if(k<0||n<0||k>n)throw Error(p,"math.comb needs 0 <= k <= n.");k=std::min(k,n-k);std::int64_t r=1;for(std::int64_t i=1;i<=k;++i)r=r*(n-k+i)/i;return Value(r);});m->exports["perm"]=callable("math.perm",2,2,[](const std::vector<Value>&a,SourcePos p){auto n=integer(a[0],p,"math.perm"),k=integer(a[1],p,"math.perm");if(k<0||n<0||k>n||n>20)throw Error(p,"math.perm needs 0 <= k <= n <= 20.");std::int64_t r=1;for(std::int64_t i=0;i<k;++i)r*=n-i;return Value(r);});
    auto stat=[&](const std::string& n,auto f){m->exports[n]=callable("math."+n,1,1,[n,f](const std::vector<Value>&a,SourcePos p){auto v=numeric_list(a[0],p,"math."+n);return Value(f(std::move(v)));});};
    stat("sum",[](std::vector<double> v){return std::accumulate(v.begin(),v.end(),0.0);});stat("mean",[](std::vector<double> v){return std::accumulate(v.begin(),v.end(),0.0)/static_cast<double>(v.size());});stat("median",[](std::vector<double> v){std::sort(v.begin(),v.end());auto n=v.size();return n%2?v[n/2]:(v[n/2-1]+v[n/2])/2.0;});stat("variance",[](std::vector<double> v){auto mean=std::accumulate(v.begin(),v.end(),0.0)/v.size();double s=0;for(auto x:v){auto d=x-mean;s+=d*d;}return s/v.size();});stat("stddev",[](std::vector<double> v){auto mean=std::accumulate(v.begin(),v.end(),0.0)/v.size();double s=0;for(auto x:v){auto d=x-mean;s+=d*d;}return std::sqrt(s/v.size());});
  }else if(name=="data"){
    m->exports["append"]=callable("data.append",2,2,[](const std::vector<Value>&a,SourcePos p){list(a[0],p,"data.append")->items.push_back(a[1]);return Value{};});m->exports["extend"]=callable("data.extend",2,2,[](const std::vector<Value>&a,SourcePos p){auto dst=list(a[0],p,"data.extend"),src=list(a[1],p,"data.extend");dst->items.insert(dst->items.end(),src->items.begin(),src->items.end());return Value{};});m->exports["insert"]=callable("data.insert",3,3,[](const std::vector<Value>&a,SourcePos p){auto l=list(a[0],p,"data.insert");auto i=integer(a[1],p,"data.insert");if(i<0)i+=static_cast<std::int64_t>(l->items.size());i=std::clamp<std::int64_t>(i,0,static_cast<std::int64_t>(l->items.size()));l->items.insert(l->items.begin()+i,a[2]);return Value{};});m->exports["pop"]=callable("data.pop",1,2,[](const std::vector<Value>&a,SourcePos p){auto l=list(a[0],p,"data.pop");if(l->items.empty())throw Error(p,"data.pop cannot pop an empty List.");auto i=a.size()==2?integer(a[1],p,"data.pop"):static_cast<std::int64_t>(l->items.size()-1);if(i<0)i+=static_cast<std::int64_t>(l->items.size());if(i<0||static_cast<std::size_t>(i)>=l->items.size())throw Error(p,"data.pop index is out of bounds.");auto v=l->items[static_cast<std::size_t>(i)];l->items.erase(l->items.begin()+i);return v;},true);
    m->exports["clear"]=callable("data.clear",1,1,[](const std::vector<Value>&a,SourcePos p){if(auto l=std::get_if<std::shared_ptr<ListData>>(&a[0].data())){(*l)->items.clear();return Value{};}if(auto mp=std::get_if<std::shared_ptr<MapData>>(&a[0].data())){(*mp)->items.clear();return Value{};}if(auto s=std::get_if<std::shared_ptr<SetData>>(&a[0].data())){(*s)->items.clear();return Value{};}throw Error(p,"data.clear needs List, Map, or Set.");});m->exports["copy"]=callable("data.copy",1,1,[](const std::vector<Value>&a,SourcePos p){if(auto l=std::get_if<std::shared_ptr<ListData>>(&a[0].data())){auto x=std::make_shared<ListData>();x->items=(*l)->items;return Value(x);}if(auto mp=std::get_if<std::shared_ptr<MapData>>(&a[0].data())){auto x=std::make_shared<MapData>();x->items=(*mp)->items;return Value(x);}if(auto s=std::get_if<std::shared_ptr<SetData>>(&a[0].data())){auto x=std::make_shared<SetData>();x->items=(*s)->items;return Value(x);}throw Error(p,"data.copy needs List, Map, or Set.");});
    m->exports["get"]=callable("data.get",2,3,[](const std::vector<Value>&a,SourcePos p){auto mp=map_value(a[0],p,"data.get"),k=text(a[1],p,"data.get");for(auto& [key,v]:mp->items)if(key==k)return v;if(a.size()==3)return a[2];return Value{};},true);m->exports["set"]=callable("data.set",3,3,[](const std::vector<Value>&a,SourcePos p){auto mp=map_value(a[0],p,"data.set"),k=text(a[1],p,"data.set");for(auto& [key,v]:mp->items)if(key==k){v=a[2];return Value{};}mp->items.emplace_back(k,a[2]);return Value{};});m->exports["update"]=callable("data.update",2,2,[](const std::vector<Value>&a,SourcePos p){auto dst=map_value(a[0],p,"data.update"),src=map_value(a[1],p,"data.update");for(auto& [k,v]:src->items){bool found=false;for(auto& [dk,dv]:dst->items)if(dk==k){dv=v;found=true;break;}if(!found)dst->items.emplace_back(k,v);}return Value{};});m->exports["delete"]=callable("data.delete",2,2,[](const std::vector<Value>&a,SourcePos p){auto mp=map_value(a[0],p,"data.delete"),k=text(a[1],p,"data.delete");auto i=std::find_if(mp->items.begin(),mp->items.end(),[&](auto& q){return q.first==k;});if(i==mp->items.end())return Value(false);mp->items.erase(i);return Value(true);});m->exports["has"]=callable("data.has",2,2,[](const std::vector<Value>&a,SourcePos p){if(auto l=std::get_if<std::shared_ptr<ListData>>(&a[0].data())){for(auto& v:(*l)->items)if(value_equal(v,a[1]))return Value(true);return Value(false);}if(auto mp=std::get_if<std::shared_ptr<MapData>>(&a[0].data())){auto k=text(a[1],p,"data.has");for(auto& [key,v]:(*mp)->items){(void)v;if(key==k)return Value(true);}return Value(false);}throw Error(p,"data.has needs List or Map.");});
    m->exports["keys"]=callable("data.keys",1,1,[](const std::vector<Value>&a,SourcePos p){auto mp=map_value(a[0],p,"data.keys"),out=std::make_shared<ListData>();for(auto& [k,v]:mp->items){(void)v;out->items.emplace_back(k);}return Value(out);});m->exports["values"]=callable("data.values",1,1,[](const std::vector<Value>&a,SourcePos p){auto mp=map_value(a[0],p,"data.values"),out=std::make_shared<ListData>();for(auto& [k,v]:mp->items){(void)k;out->items.push_back(v);}return Value(out);});m->exports["items"]=callable("data.items",1,1,[](const std::vector<Value>&a,SourcePos p){auto mp=map_value(a[0],p,"data.items"),out=std::make_shared<ListData>();for(auto& [k,v]:mp->items){auto pair=std::make_shared<ListData>();pair->items.emplace_back(k);pair->items.push_back(v);out->items.emplace_back(pair);}return Value(out);});
  }else if(name=="net"){
    m->exports["get"]=callable("net.get",1,1,[](const std::vector<Value>&a,SourcePos p){return Value(curl_request("GET",text(a[0],p,"net.get"),"","text/plain",p).body);});m->exports["post"]=callable("net.post",2,2,[](const std::vector<Value>&a,SourcePos p){return Value(curl_request("POST",text(a[0],p,"net.post"),text(a[1],p,"net.post"),"text/plain; charset=utf-8",p).body);});m->exports["post_json"]=callable("net.post_json",2,2,[](const std::vector<Value>&a,SourcePos p){return Value(curl_request("POST",text(a[0],p,"net.post_json"),text(a[1],p,"net.post_json"),"application/json",p).body);});m->exports["request"]=callable("net.request",3,3,[](const std::vector<Value>&a,SourcePos p){return response_map(curl_request(text(a[0],p,"net.request"),text(a[1],p,"net.request"),text(a[2],p,"net.request"),"text/plain; charset=utf-8",p));});m->exports["download"]=callable("net.download",2,2,[](const std::vector<Value>&a,SourcePos p){auto url=text(a[0],p,"net.download"),path=text(a[1],p,"net.download");auto code=normalized_system("curl -fL --max-time 60 -o "+shell_quote(path)+" "+shell_quote(url));if(code!=0)throw Error(p,"net.download failed.");return Value{};});
  }else if(name=="node"){
    m->exports["version"]=callable("node.version",0,0,[](const std::vector<Value>&,SourcePos p){return Value(process_output("node --version",p));});m->exports["run"]=callable("node.run",1,1,[](const std::vector<Value>&a,SourcePos p){return Value(static_cast<std::int64_t>(normalized_system("node "+shell_quote(text(a[0],p,"node.run")))));});m->exports["output"]=callable("node.output",1,1,[](const std::vector<Value>&a,SourcePos p){return Value(process_output("node "+shell_quote(text(a[0],p,"node.output")),p));});m->exports["eval"]=callable("node.eval",1,1,[](const std::vector<Value>&a,SourcePos p){return Value(process_output("node -e "+shell_quote(text(a[0],p,"node.eval")),p));});m->exports["npm"]=callable("node.npm",1,1,[](const std::vector<Value>&a,SourcePos p){return Value(static_cast<std::int64_t>(normalized_system("npm "+text(a[0],p,"node.npm"))));});m->exports["npx"]=callable("node.npx",1,1,[](const std::vector<Value>&a,SourcePos p){return Value(static_cast<std::int64_t>(normalized_system("npx "+text(a[0],p,"node.npx"))));});
  }else if(name=="next"){
    auto run_in=[](const std::string& project,const std::string& command){return "cd "+shell_quote(project)+" && "+command;};m->exports["create"]=callable("next.create",1,1,[](const std::vector<Value>&a,SourcePos p){return Value(static_cast<std::int64_t>(normalized_system("npx create-next-app@latest "+shell_quote(text(a[0],p,"next.create")))));});m->exports["dev"]=callable("next.dev",1,1,[run_in](const std::vector<Value>&a,SourcePos p){return Value(static_cast<std::int64_t>(normalized_system(run_in(text(a[0],p,"next.dev"),"npm run dev"))));});m->exports["build"]=callable("next.build",1,1,[run_in](const std::vector<Value>&a,SourcePos p){return Value(static_cast<std::int64_t>(normalized_system(run_in(text(a[0],p,"next.build"),"npm run build"))));});m->exports["start"]=callable("next.start",1,1,[run_in](const std::vector<Value>&a,SourcePos p){return Value(static_cast<std::int64_t>(normalized_system(run_in(text(a[0],p,"next.start"),"npm run start"))));});m->exports["lint"]=callable("next.lint",1,1,[run_in](const std::vector<Value>&a,SourcePos p){return Value(static_cast<std::int64_t>(normalized_system(run_in(text(a[0],p,"next.lint"),"npm run lint"))));});
  }else if(name=="game"){
    m->exports["new"]=callable("game.new",3,3,[](const std::vector<Value>&a,SourcePos p){auto w=integer(a[0],p,"game.new"),h=integer(a[1],p,"game.new");if(w<1||h<1||w>8192||h>8192)throw Error(p,"game.new size must be between 1 and 8192.");auto id=next_scene()++;scenes()[id]=GameScene{static_cast<int>(w),static_cast<int>(h),text(a[2],p,"game.new")};return Value(id);});m->exports["background"]=callable("game.background",2,2,[](const std::vector<Value>&a,SourcePos p){scene_for(integer(a[0],p,"game.background"),p).background=text(a[1],p,"game.background");return Value{};});m->exports["clear"]=callable("game.clear",1,1,[](const std::vector<Value>&a,SourcePos p){scene_for(integer(a[0],p,"game.clear"),p).draw.clear();return Value{};});
    m->exports["rect"]=callable("game.rect",7,7,[](const std::vector<Value>&a,SourcePos p){auto& s=scene_for(integer(a[0],p,"game.rect"),p);auto x=number(a[1],p,"game.rect"),y=number(a[2],p,"game.rect"),w=number(a[3],p,"game.rect"),h=number(a[4],p,"game.rect");auto color=js_escape(text(a[5],p,"game.rect"));auto fill=boolean(a[6],p,"game.rect");std::ostringstream q;q<<"ctx."<<(fill?"fillStyle":"strokeStyle")<<"=\""<<color<<"\";ctx."<<(fill?"fillRect":"strokeRect")<<"("<<x<<","<<y<<","<<w<<","<<h<<");";s.draw.push_back(q.str());return Value{};});m->exports["circle"]=callable("game.circle",6,6,[](const std::vector<Value>&a,SourcePos p){auto& s=scene_for(integer(a[0],p,"game.circle"),p);auto x=number(a[1],p,"game.circle"),y=number(a[2],p,"game.circle"),r=number(a[3],p,"game.circle");auto color=js_escape(text(a[4],p,"game.circle"));auto fill=boolean(a[5],p,"game.circle");std::ostringstream q;q<<"ctx.beginPath();ctx.arc("<<x<<","<<y<<","<<r<<",0,Math.PI*2);ctx."<<(fill?"fillStyle":"strokeStyle")<<"=\""<<color<<"\";ctx."<<(fill?"fill()":"stroke()")<<";";s.draw.push_back(q.str());return Value{};});m->exports["line"]=callable("game.line",7,7,[](const std::vector<Value>&a,SourcePos p){auto& s=scene_for(integer(a[0],p,"game.line"),p);std::ostringstream q;q<<"ctx.beginPath();ctx.moveTo("<<number(a[1],p,"game.line")<<","<<number(a[2],p,"game.line")<<");ctx.lineTo("<<number(a[3],p,"game.line")<<","<<number(a[4],p,"game.line")<<");ctx.strokeStyle=\""<<js_escape(text(a[5],p,"game.line"))<<"\";ctx.lineWidth="<<number(a[6],p,"game.line")<<";ctx.stroke();";s.draw.push_back(q.str());return Value{};});m->exports["text"]=callable("game.text",6,6,[](const std::vector<Value>&a,SourcePos p){auto& s=scene_for(integer(a[0],p,"game.text"),p);std::ostringstream q;q<<"ctx.fillStyle=\""<<js_escape(text(a[5],p,"game.text"))<<"\";ctx.font=\""<<number(a[4],p,"game.text")<<"px system-ui\";ctx.fillText(\""<<js_escape(text(a[1],p,"game.text"))<<"\","<<number(a[2],p,"game.text")<<","<<number(a[3],p,"game.text")<<");";s.draw.push_back(q.str());return Value{};});m->exports["script"]=callable("game.script",2,2,[](const std::vector<Value>&a,SourcePos p){scene_for(integer(a[0],p,"game.script"),p).scripts.push_back(text(a[1],p,"game.script"));return Value{};});m->exports["html"]=callable("game.html",1,1,[](const std::vector<Value>&a,SourcePos p){return Value(scene_html(integer(a[0],p,"game.html"),p));});m->exports["save"]=callable("game.save",2,2,[](const std::vector<Value>&a,SourcePos p){auto path=std::filesystem::path(text(a[1],p,"game.save"));if(!path.parent_path().empty())std::filesystem::create_directories(path.parent_path());std::ofstream out(path,std::ios::binary|std::ios::trunc);if(!out)throw Error(p,"game.save could not write '"+path.string()+"'.");out<<scene_html(integer(a[0],p,"game.save"),p);return Value{};});m->exports["show"]=callable("game.show",1,1,[](const std::vector<Value>&a,SourcePos p){auto id=integer(a[0],p,"game.show");auto path=std::filesystem::temp_directory_path()/("se-game-"+std::to_string(id)+".html");std::ofstream out(path,std::ios::binary|std::ios::trunc);if(!out)throw Error(p,"game.show could not create a temporary display file.");out<<scene_html(id,p);out.close();open_file(path);return Value{};});
  }
  return m;
}

} // namespace s
