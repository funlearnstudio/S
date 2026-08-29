#include "s/compiler.hpp"
#include <iomanip>
#include <sstream>

namespace s {
static std::string quote(const std::string&s){std::ostringstream o;o<<std::quoted(s);return o.str();}
std::string CppCompiler::expr(const ast::ExprPtr&e){
  if(auto x=std::dynamic_pointer_cast<ast::Literal>(e))return std::visit([](auto v)->std::string{using T=decltype(v);if constexpr(std::is_same_v<T,std::string>)return "Value("+quote(v)+")";else if constexpr(std::is_same_v<T,bool>)return std::string("Value(")+(v?"true":"false")+")";else if constexpr(std::is_same_v<T,std::int64_t>)return "Value(int64_t{"+std::to_string(v)+"})";else return "Value("+std::to_string(v)+")";},x->value);
  if(auto x=std::dynamic_pointer_cast<ast::Variable>(e))return "get("+env_+","+quote(x->name)+","+std::to_string(x->pos.line)+")";
  if(auto x=std::dynamic_pointer_cast<ast::Binary>(e))return "bin("+quote(std::string(token_name(x->op)))+","+expr(x->left)+","+expr(x->right)+","+std::to_string(x->pos.line)+")";
  if(auto x=std::dynamic_pointer_cast<ast::Unary>(e))return "unary("+quote(std::string(token_name(x->op)))+","+expr(x->value)+","+std::to_string(x->pos.line)+")";
  if(auto x=std::dynamic_pointer_cast<ast::List>(e)){std::string r="list({";for(std::size_t i=0;i<x->items.size();++i){if(i)r+=",";r+=expr(x->items[i]);}return r+"})";}
  if(auto x=std::dynamic_pointer_cast<ast::Index>(e))return "index("+expr(x->value)+","+expr(x->index)+","+std::to_string(x->pos.line)+")";
  if(auto x=std::dynamic_pointer_cast<ast::Member>(e))return "member("+expr(x->value)+","+quote(x->name)+","+std::to_string(x->pos.line)+")";
  if(auto x=std::dynamic_pointer_cast<ast::Ask>(e))return "ask("+expr(x->question)+")";
  if(auto x=std::dynamic_pointer_cast<ast::Range>(e))return "range("+expr(x->start)+","+expr(x->end)+","+std::to_string(x->pos.line)+")";
  if(auto x=std::dynamic_pointer_cast<ast::Call>(e)){std::string r="call("+expr(x->callee)+",{";for(std::size_t i=0;i<x->args.size();++i){if(i)r+=",";r+=expr(x->args[i]);}return r+"},"+std::to_string(x->pos.line)+")";}
  return "Value()";
}
void CppCompiler::block(const ast::Block&b){for(auto&s:b)stmt(s);}
void CppCompiler::stmt(const ast::StmtPtr&s){
  if(auto x=std::dynamic_pointer_cast<ast::Say>(s)){out_+=pad()+"say("+expr(x->value)+");\n";return;}
  if(auto x=std::dynamic_pointer_cast<ast::ExprStmt>(s)){out_+=pad()+"(void)"+expr(x->value)+";\n";return;}
  if(auto x=std::dynamic_pointer_cast<ast::Assign>(s)){if(auto n=std::dynamic_pointer_cast<ast::Variable>(x->target)){out_+=pad()+"set("+env_+","+quote(n->name)+","+expr(x->value)+");\n";return;}if(auto i=std::dynamic_pointer_cast<ast::Index>(x->target)){out_+=pad()+"set_index("+expr(i->value)+","+expr(i->index)+","+expr(x->value)+","+std::to_string(x->pos.line)+");\n";return;}}
  if(auto x=std::dynamic_pointer_cast<ast::If>(s)){out_+=pad()+"if (truth("+expr(x->condition)+","+std::to_string(x->pos.line)+")) {\n";++indent_;block(x->then_block);--indent_;out_+=pad()+"}";if(!x->else_block.empty()){out_+=" else {\n";++indent_;block(x->else_block);--indent_;out_+=pad()+"}";}out_+="\n";return;}
  if(auto x=std::dynamic_pointer_cast<ast::Repeat>(s)){auto n=fresh();out_+=pad()+"for(int64_t "+n+"=0; "+n+"<as_int("+expr(x->count)+","+std::to_string(x->pos.line)+"); ++"+n+") {\n";++indent_;block(x->body);--indent_;out_+=pad()+"}\n";return;}
  if(auto x=std::dynamic_pointer_cast<ast::For>(s)){auto values=fresh(),item=fresh();out_+=pad()+"auto "+values+"="+expr(x->values)+";\n";out_+=pad()+"for(const auto& "+item+" : as_list("+values+","+std::to_string(x->pos.line)+")) {\n";++indent_;out_+=pad()+"set("+env_+","+quote(x->name)+","+item+");\n";block(x->body);--indent_;out_+=pad()+"}\n";return;}
  if(auto x=std::dynamic_pointer_cast<ast::While>(s)){out_+=pad()+"while(truth("+expr(x->condition)+","+std::to_string(x->pos.line)+")) {\n";++indent_;block(x->body);--indent_;out_+=pad()+"}\n";return;}
  if(auto x=std::dynamic_pointer_cast<ast::Function>(s)){auto old=env_,local=fresh(),args=fresh();out_+=pad()+"set("+env_+","+quote(x->name)+",func([&](std::vector<Value> "+args+") -> Value {\n";++indent_;out_+=pad()+"if("+args+".size()!="+std::to_string(x->params.size())+") fail("+std::to_string(x->pos.line)+",\"Wrong number of function values.\");\n";out_+=pad()+"Env "+local+"="+env_+";\n";env_=local;for(std::size_t i=0;i<x->params.size();++i)out_+=pad()+"set("+env_+","+quote(x->params[i])+","+args+"["+std::to_string(i)+"]);\n";block(x->body);out_+=pad()+"return Value();\n";env_=old;--indent_;out_+=pad()+"}));\n";return;}
  if(auto x=std::dynamic_pointer_cast<ast::Give>(s)){out_+=pad()+"return "+expr(x->value)+";\n";return;}
}
std::string CppCompiler::generate(const ast::Program&p){
  out_=R"CPP(#include <algorithm>
#include <cmath>
#include <cstdint>
#include <functional>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
struct Value; using List=std::shared_ptr<std::vector<Value>>; using Func=std::shared_ptr<std::function<Value(std::vector<Value>)>>;
struct Value{using Data=std::variant<std::monostate,int64_t,double,bool,std::string,List,Func>;Data d;Value()=default;template<class T>Value(T v):d(std::move(v)){};};
using Env=std::unordered_map<std::string,Value>;
[[noreturn]] void fail(int line,const std::string&m){throw std::runtime_error("S error on line "+std::to_string(line)+": "+m);}
Value get(Env&e,const std::string&n,int line){auto i=e.find(n);if(i==e.end())fail(line,"Unknown name '"+n+"'.");return i->second;}void set(Env&e,const std::string&n,Value v){e[n]=std::move(v);}
bool is_num(const Value&v){return std::holds_alternative<int64_t>(v.d)||std::holds_alternative<double>(v.d);}double num(const Value&v,int l){if(auto n=std::get_if<int64_t>(&v.d))return *n;if(auto n=std::get_if<double>(&v.d))return *n;fail(l,"A number was expected.");}
int64_t as_int(const Value&v,int l){if(auto n=std::get_if<int64_t>(&v.d))return *n;fail(l,"An Int was expected.");}bool truth(const Value&v,int l){if(auto b=std::get_if<bool>(&v.d))return *b;fail(l,"A Bool was expected.");}
std::string text(const Value&v){if(auto n=std::get_if<int64_t>(&v.d))return std::to_string(*n);if(auto n=std::get_if<double>(&v.d))return std::to_string(*n);if(auto b=std::get_if<bool>(&v.d))return *b?"true":"false";if(auto s=std::get_if<std::string>(&v.d))return *s;if(auto a=std::get_if<List>(&v.d)){std::string r="[";for(size_t i=0;i<(*a)->size();++i){if(i)r+=", ";r+=text((**a)[i]);}return r+"]";}return "none";}
Value list(std::vector<Value>v){return std::make_shared<std::vector<Value>>(std::move(v));}Value func(std::function<Value(std::vector<Value>)>f){return std::make_shared<std::function<Value(std::vector<Value>)>>(std::move(f));}
const std::vector<Value>&as_list(const Value&v,int l){if(auto a=std::get_if<List>(&v.d))return **a;fail(l,"A List was expected.");}
Value bin(const std::string&o,const Value&a,const Value&b,int l){if(o=="Plus"&&std::holds_alternative<std::string>(a.d)&&std::holds_alternative<std::string>(b.d))return std::get<std::string>(a.d)+std::get<std::string>(b.d);if(o=="EqualEqual"||o=="BangEqual"){bool q=is_num(a)&&is_num(b)?num(a,l)==num(b,l):a.d.index()==b.d.index()&&text(a)==text(b);return o=="EqualEqual"?q:!q;}if(o=="And")return truth(a,l)&&truth(b,l);if(o=="Or")return truth(a,l)||truth(b,l);bool ints=std::holds_alternative<int64_t>(a.d)&&std::holds_alternative<int64_t>(b.d);double x=num(a,l),y=num(b,l);if(o=="Plus")return ints?Value(std::get<int64_t>(a.d)+std::get<int64_t>(b.d)):Value(x+y);if(o=="Minus")return ints?Value(std::get<int64_t>(a.d)-std::get<int64_t>(b.d)):Value(x-y);if(o=="Star")return ints?Value(std::get<int64_t>(a.d)*std::get<int64_t>(b.d)):Value(x*y);if(o=="Slash"){if(y==0)fail(l,"Division by zero.");return x/y;}if(o=="Percent"){if(y==0)fail(l,"Division by zero.");return ints?Value(std::get<int64_t>(a.d)%std::get<int64_t>(b.d)):Value(std::fmod(x,y));}if(o=="Power")return std::pow(x,y);if(o=="Greater")return x>y;if(o=="Less")return x<y;if(o=="GreaterEqual")return x>=y;if(o=="LessEqual")return x<=y;fail(l,"Unsupported operation.");}
Value unary(const std::string&o,const Value&v,int l){if(o=="Not")return !truth(v,l);if(auto n=std::get_if<int64_t>(&v.d))return -*n;return -num(v,l);}Value index(Value v,Value i,int l){auto&a=as_list(v,l);auto n=as_int(i,l);if(n<0||size_t(n)>=a.size())fail(l,"List index is out of bounds.");return a[n];}
void set_index(Value v,Value i,Value x,int l){auto a=std::get_if<List>(&v.d);if(!a)fail(l,"A List was expected.");auto n=as_int(i,l);if(n<0||size_t(n)>=(**a).size())fail(l,"List index is out of bounds.");(**a)[n]=std::move(x);}Value member(Value v,const std::string&n,int l){if(n=="len"){if(auto s=std::get_if<std::string>(&v.d))return int64_t(s->size());if(auto a=std::get_if<List>(&v.d))return int64_t((*a)->size());}if((n=="upper"||n=="lower")&&std::holds_alternative<std::string>(v.d)){auto s=std::get<std::string>(v.d);for(char&c:s)c=n=="upper"?char(std::toupper(c)):char(std::tolower(c));return s;}fail(l,"Unknown member '"+n+"'.");}
Value range(Value a,Value b,int l){int64_t x=as_int(a,l),y=as_int(b,l),step=x<=y?1:-1;std::vector<Value>r;for(;;x+=step){r.emplace_back(x);if(x==y)break;}return list(std::move(r));}Value call(Value v,std::vector<Value>a,int l){if(auto f=std::get_if<Func>(&v.d))return (**f)(std::move(a));fail(l,"Only a function can be called.");}
Value ask(Value q){std::cout<<text(q)<<": ";std::string s;std::getline(std::cin,s);return s;}void say(const Value&v){std::cout<<text(v)<<'\n';}
int main(){try{Env env;
)CPP";
  indent_=1;env_="env";block(p.statements);out_+="  return 0;\n}catch(const std::exception&e){std::cerr<<e.what()<<'\\n';return 1;}\n}\n";return out_;
}
} // namespace s
