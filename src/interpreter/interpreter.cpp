#include "s/interpreter.hpp"
#include "s/ffi.hpp"
#include <algorithm>
#include <cctype>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <thread>

namespace s {
namespace {
struct ReturnSignal{Value value;};
std::shared_ptr<CallableData> callable(std::string name,std::size_t min,std::size_t max,std::function<Value(const std::vector<Value>&,SourcePos)> f,bool variadic=false){auto c=std::make_shared<CallableData>();c->name=std::move(name);c->min_args=min;c->max_args=max;c->variadic=variadic;c->call=std::move(f);return c;}
}

Interpreter::Interpreter(std::istream& i,std::ostream& o):in_(i),out_(o),env_(std::make_shared<Environment>()){install_builtins(env_);}
[[noreturn]] void Interpreter::runtime_fail(SourcePos p,const std::string&m,const std::string&kind) const{throw RuntimeFailure({m,source_,p.line,kind});}
void Interpreter::execute_block(const ast::Block& b,std::shared_ptr<Environment> e){auto old=env_;env_=std::move(e);try{for(auto&s:b)execute(s);}catch(...){env_=old;throw;}env_=old;}

void Interpreter::install_builtins(const std::shared_ptr<Environment>&e){
  e->define("read",callable("read",1,1,[this](const std::vector<Value>&a,SourcePos p){auto path=path_text(a[0],p);std::ifstream f(path,std::ios::binary);if(!f)runtime_fail(p,"Could not read '"+path+"'.","FileError");return Value(std::string(std::istreambuf_iterator<char>(f),{}));}));
  e->define("write",callable("write",2,2,[this](const std::vector<Value>&a,SourcePos p){auto path=path_text(a[0],p);if(!std::holds_alternative<std::string>(a[1].data()))runtime_fail(p,"write needs Text.","FileError");std::ofstream f(path,std::ios::binary|std::ios::trunc);if(!f)runtime_fail(p,"Could not write '"+path+"'.","FileError");f<<std::get<std::string>(a[1].data());if(!f)runtime_fail(p,"Writing '"+path+"' failed.","FileError");return Value{};}));
  e->define("append",callable("append",2,2,[this](const std::vector<Value>&a,SourcePos p){auto path=path_text(a[0],p);if(!std::holds_alternative<std::string>(a[1].data()))runtime_fail(p,"append needs Text.","FileError");std::ofstream f(path,std::ios::binary|std::ios::app);if(!f)runtime_fail(p,"Could not append to '"+path+"'.","FileError");f<<std::get<std::string>(a[1].data());if(!f)runtime_fail(p,"Appending to '"+path+"' failed.","FileError");return Value{};}));
  e->define("open",callable("open",1,1,[this](const std::vector<Value>&a,SourcePos p){auto path=path_text(a[0],p);auto file=std::make_shared<FileData>();file->path=path;file->stream=std::make_shared<std::fstream>(path,std::ios::in|std::ios::out|std::ios::binary);if(!file->stream->is_open())runtime_fail(p,"Could not open '"+path+"'.","FileError");return Value(file);}));
  e->define("wait",callable("wait",1,1,[this](const std::vector<Value>&a,SourcePos p){auto d=std::get_if<DurationData>(&a[0].data());if(!d)runtime_fail(p,"wait needs a Duration.","TimeError");if(d->milliseconds<0)runtime_fail(p,"wait duration cannot be negative.","TimeError");std::this_thread::sleep_for(std::chrono::milliseconds(d->milliseconds));return Value{};}));
}

std::shared_ptr<ModuleData> Interpreter::builtin_module(const std::string&name){
  auto m=std::make_shared<ModuleData>();m->name=name;
  if(name=="path"){
    m->exports["join"]=callable("path.join",1,64,[](const std::vector<Value>&a,SourcePos p){std::filesystem::path r;for(auto&v:a)r/=path_text(v,p);return Value(PathData{r});},true);
    m->exports["name"]=callable("path.name",1,1,[](const std::vector<Value>&a,SourcePos p){return Value(std::filesystem::path(path_text(a[0],p)).filename().string());});
    m->exports["ext"]=callable("path.ext",1,1,[](const std::vector<Value>&a,SourcePos p){return Value(std::filesystem::path(path_text(a[0],p)).extension().string());});
    m->exports["parent"]=callable("path.parent",1,1,[](const std::vector<Value>&a,SourcePos p){return Value(PathData{std::filesystem::path(path_text(a[0],p)).parent_path()});});
    m->exports["exists"]=callable("path.exists",1,1,[](const std::vector<Value>&a,SourcePos p){std::error_code ec;auto r=std::filesystem::exists(path_text(a[0],p),ec);return Value(!ec&&r);});
    m->exports["is_file"]=callable("path.is_file",1,1,[](const std::vector<Value>&a,SourcePos p){std::error_code ec;auto r=std::filesystem::is_regular_file(path_text(a[0],p),ec);return Value(!ec&&r);});
    m->exports["is_dir"]=callable("path.is_dir",1,1,[](const std::vector<Value>&a,SourcePos p){std::error_code ec;auto r=std::filesystem::is_directory(path_text(a[0],p),ec);return Value(!ec&&r);});
  }else if(name=="time"){
    m->exports["now"]=callable("time.now",0,0,[](const std::vector<Value>&,SourcePos){return Value(TimeData{std::chrono::system_clock::now()});});
  }else if(name=="file"){
    m->exports["read"]=env_->get("read",{});m->exports["write"]=env_->get("write",{});m->exports["append"]=env_->get("append",{});m->exports["open"]=env_->get("open",{});
  }
  return m;
}

Value Interpreter::call(Value c,const std::vector<Value>&args,SourcePos p){
  auto f=std::get_if<std::shared_ptr<CallableData>>(&c.data());
  if(!f)throw Error(p,"Only a function or method can be called.");
  if((!(*f)->variadic&&args.size()!=(*f)->min_args)||((*f)->variadic&&args.size()<(*f)->min_args))
    throw Error(p,"'"+(*f)->name+"' needs "+std::to_string((*f)->min_args)+((*f)->variadic?" or more":"")+" values, but got "+std::to_string(args.size())+".");
  return (*f)->call(args,p);
}

Value Interpreter::instantiate(const std::shared_ptr<TypeData>&type,SourcePos p){
  auto object=std::make_shared<ObjectData>();object->type=type;auto local=std::make_shared<Environment>(type->closure,object);auto old=env_;env_=local;
  try{for(auto&f:type->fields)object->fields[f.name]=evaluate(f.value);}catch(...){env_=old;throw;}env_=old;(void)p;return object;
}

Value Interpreter::member(Value v,const std::string&name,SourcePos p,bool auto_call){
  if(name=="len"){
    if(auto t=std::get_if<std::string>(&v.data()))return static_cast<std::int64_t>(t->size());
    if(auto l=std::get_if<std::shared_ptr<ListData>>(&v.data()))return static_cast<std::int64_t>((*l)->items.size());
    if(auto m=std::get_if<std::shared_ptr<MapData>>(&v.data()))return static_cast<std::int64_t>((*m)->items.size());
    if(auto s=std::get_if<std::shared_ptr<SetData>>(&v.data()))return static_cast<std::int64_t>((*s)->items.size());
  }
  if((name=="upper"||name=="lower")&&std::holds_alternative<std::string>(v.data())){auto t=std::get<std::string>(v.data());std::transform(t.begin(),t.end(),t.begin(),[&](unsigned char c){return static_cast<char>(name=="upper"?std::toupper(c):std::tolower(c));});return t;}
  if(auto l=std::get_if<std::shared_ptr<ListData>>(&v.data())){
    if(name=="add")return callable("List.add",1,1,[list=*l](const std::vector<Value>&a,SourcePos){list->items.push_back(a[0]);return Value{};});
    if(name=="remove")return callable("List.remove",1,1,[list=*l](const std::vector<Value>&a,SourcePos){auto i=std::find_if(list->items.begin(),list->items.end(),[&](const Value&x){return value_equal(x,a[0]);});if(i==list->items.end())return Value(false);list->items.erase(i);return Value(true);});
  }
  if(auto s=std::get_if<std::shared_ptr<SetData>>(&v.data())){
    if(name=="add")return callable("Set.add",1,1,[set=*s](const std::vector<Value>&a,SourcePos){for(auto&x:set->items)if(value_equal(x,a[0]))return Value{};set->items.push_back(a[0]);return Value{};});
    if(name=="remove")return callable("Set.remove",1,1,[set=*s](const std::vector<Value>&a,SourcePos){auto i=std::find_if(set->items.begin(),set->items.end(),[&](const Value&x){return value_equal(x,a[0]);});if(i==set->items.end())return Value(false);set->items.erase(i);return Value(true);});
  }
  if(auto o=std::get_if<std::shared_ptr<ObjectData>>(&v.data())){
    if(auto f=(*o)->fields.find(name);f!=(*o)->fields.end())return f->second;
    auto type=(*o)->type;if(type){if(auto m=type->methods.find(name);m!=type->methods.end()){auto method=m->second;auto object=*o;auto c=callable(type->name+"."+name,method->params.size(),method->params.size(),[this,object,method,type](const std::vector<Value>&a,SourcePos)->Value{auto local=std::make_shared<Environment>(type->closure,object);for(std::size_t i=0;i<a.size();++i)local->define(method->params[i],a[i]);try{execute_block(method->body,local);}catch(ReturnSignal&r){return r.value;}return {};});if(auto_call&&method->params.empty())return call(c,{},p);return c;}}
  }
  if(auto m=std::get_if<std::shared_ptr<ModuleData>>(&v.data())){if(auto x=(*m)->exports.find(name);x!=(*m)->exports.end()){auto r=x->second;if(auto_call)if(auto c=std::get_if<std::shared_ptr<CallableData>>(&r.data());c&&(*c)->min_args==0&&!(*c)->variadic)return call(r,{},p);return r;}}
  if(auto f=std::get_if<std::shared_ptr<FileData>>(&v.data())){
    auto file=*f;
    if(name=="read"){auto c=callable("File.read",0,0,[this,file](const std::vector<Value>&,SourcePos q){if(file->closed||!file->stream||!file->stream->is_open())runtime_fail(q,"This file is closed.","FileError");file->stream->clear();file->stream->seekg(0);return Value(std::string(std::istreambuf_iterator<char>(*file->stream),{}));});return auto_call?call(c,{},p):Value(c);}
    if(name=="close"){auto c=callable("File.close",0,0,[file](const std::vector<Value>&,SourcePos){if(!file->closed&&file->stream&&file->stream->is_open())file->stream->close();file->closed=true;return Value{};});return auto_call?call(c,{},p):Value(c);}
    if(name=="write")return callable("File.write",1,1,[this,file](const std::vector<Value>&a,SourcePos q){if(file->closed||!file->stream||!file->stream->is_open())runtime_fail(q,"This file is closed.","FileError");if(!std::holds_alternative<std::string>(a[0].data()))runtime_fail(q,"File.write needs Text.","FileError");file->stream->clear();file->stream->seekp(0,std::ios::end);*file->stream<<std::get<std::string>(a[0].data());file->stream->flush();if(!*file->stream)runtime_fail(q,"Writing the file failed.","FileError");return Value{};});
  }
  if(auto q=std::get_if<PathData>(&v.data())){if(name=="name")return q->path.filename().string();if(name=="ext")return q->path.extension().string();if(name=="parent")return PathData{q->path.parent_path()};if(name=="exists")return std::filesystem::exists(q->path);if(name=="is_file")return std::filesystem::is_regular_file(q->path);if(name=="is_dir")return std::filesystem::is_directory(q->path);}
  if(auto e=std::get_if<std::shared_ptr<ErrorData>>(&v.data())){if(name=="message")return (*e)->message;if(name=="source")return (*e)->source;if(name=="line")return static_cast<std::int64_t>((*e)->line);if(name=="kind")return (*e)->kind;}
  throw Error(p,v.type_name()+" has no member named '"+name+"'.");
}

Value Interpreter::evaluate(const ast::ExprPtr& e){
  if(auto x=std::dynamic_pointer_cast<ast::Literal>(e))return std::visit([](auto v){return Value(v);},x->value);
  if(auto x=std::dynamic_pointer_cast<ast::Duration>(e))return DurationData{x->milliseconds};
  if(auto x=std::dynamic_pointer_cast<ast::Variable>(e)){auto v=env_->get(x->name,x->pos);if(auto t=std::get_if<std::shared_ptr<TypeData>>(&v.data()))return instantiate(*t,x->pos);if(auto f=std::get_if<std::shared_ptr<CallableData>>(&v.data());f&&(*f)->min_args==0&&!(*f)->variadic)return call(v,{},x->pos);return v;}
  if(auto x=std::dynamic_pointer_cast<ast::Unary>(e)){auto v=evaluate(x->value);if(x->op==TokenKind::Not)return !v.truth(x->pos);if(auto n=std::get_if<std::int64_t>(&v.data()))return -*n;if(auto n=std::get_if<double>(&v.data()))return -*n;throw Error(x->pos,"Only a number can follow '-'.");}
  if(auto x=std::dynamic_pointer_cast<ast::Binary>(e)){auto a=evaluate(x->left);if(x->op==TokenKind::And&&!a.truth(x->pos))return false;if(x->op==TokenKind::Or&&a.truth(x->pos))return true;return binary(x->op,a,evaluate(x->right),x->pos);}
  if(auto x=std::dynamic_pointer_cast<ast::List>(e)){auto l=std::make_shared<ListData>();for(auto&i:x->items)l->items.push_back(evaluate(i));return l;}
  if(auto x=std::dynamic_pointer_cast<ast::Set>(e)){auto s=std::make_shared<SetData>();for(auto&i:x->items){auto v=evaluate(i);bool found=false;for(auto&old:s->items)if(value_equal(old,v)){found=true;break;}if(!found)s->items.push_back(std::move(v));}return s;}
  if(auto x=std::dynamic_pointer_cast<ast::Map>(e)){auto m=std::make_shared<MapData>();for(auto&i:x->items){auto k=evaluate(i.first);if(!std::holds_alternative<std::string>(k.data()))throw Error(i.first->pos,"Map keys are Text in S 0.2.");auto key=std::get<std::string>(k.data());auto v=evaluate(i.second);auto old=std::find_if(m->items.begin(),m->items.end(),[&](auto&q){return q.first==key;});if(old==m->items.end())m->items.emplace_back(std::move(key),std::move(v));else old->second=std::move(v);}return m;}
  if(auto x=std::dynamic_pointer_cast<ast::Range>(e)){auto a=evaluate(x->start),b=evaluate(x->end);if(!std::holds_alternative<std::int64_t>(a.data())||!std::holds_alternative<std::int64_t>(b.data()))throw Error(x->pos,"A range needs two Int values.");auto l=std::make_shared<ListData>();auto from=std::get<std::int64_t>(a.data()),to=std::get<std::int64_t>(b.data());auto step=from<=to?1:-1;for(auto n=from;;n+=step){l->items.emplace_back(n);if(n==to)break;}return l;}
  if(auto x=std::dynamic_pointer_cast<ast::Index>(e)){auto v=evaluate(x->value),i=evaluate(x->index);if(auto l=std::get_if<std::shared_ptr<ListData>>(&v.data())){if(!std::holds_alternative<std::int64_t>(i.data()))throw Error(x->pos,"A List index needs Int.");auto n=std::get<std::int64_t>(i.data());if(n<0||static_cast<std::size_t>(n)>=(*l)->items.size())throw Error(x->pos,"List index "+std::to_string(n)+" is out of bounds. This list has "+std::to_string((*l)->items.size())+" items.");return (*l)->items[static_cast<std::size_t>(n)];}if(auto m=std::get_if<std::shared_ptr<MapData>>(&v.data())){if(!std::holds_alternative<std::string>(i.data()))throw Error(x->pos,"A Map key needs Text.");auto key=std::get<std::string>(i.data());for(auto&q:(*m)->items)if(q.first==key)return q.second;throw Error(x->pos,"Map has no key '"+key+"'.");}throw Error(x->pos,"Only List and Map can use [index].");}
  if(auto x=std::dynamic_pointer_cast<ast::Member>(e))return member(evaluate(x->value),x->name,x->pos,true);
  if(auto x=std::dynamic_pointer_cast<ast::Ask>(e)){auto q=evaluate(x->question);if(!std::holds_alternative<std::string>(q.data()))throw Error(x->pos,"ask needs Text.");out_<<q.text()<<": ";std::string answer;std::getline(in_,answer);return answer;}
  if(auto x=std::dynamic_pointer_cast<ast::TryExpr>(e))return evaluate(x->value);
  if(auto x=std::dynamic_pointer_cast<ast::Call>(e)){Value c;if(auto m=std::dynamic_pointer_cast<ast::Member>(x->callee))c=member(evaluate(m->value),m->name,m->pos,false);else c=env_->get(std::dynamic_pointer_cast<ast::Variable>(x->callee)?std::dynamic_pointer_cast<ast::Variable>(x->callee)->name:"",x->pos);if(!std::dynamic_pointer_cast<ast::Variable>(x->callee)&&!std::dynamic_pointer_cast<ast::Member>(x->callee))c=evaluate(x->callee);std::vector<Value>a;for(auto&i:x->args)a.push_back(evaluate(i));return call(c,a,x->pos);}
  throw Error(e->pos,"This expression is not implemented.");
}

void Interpreter::execute(const ast::StmtPtr& s){
  if(std::dynamic_pointer_cast<ast::Use>(s))return;
  if(auto x=std::dynamic_pointer_cast<ast::Say>(s)){out_<<evaluate(x->value).text()<<'\n';return;}
  if(auto x=std::dynamic_pointer_cast<ast::ExprStmt>(s)){evaluate(x->value);return;}
  if(auto x=std::dynamic_pointer_cast<ast::Assign>(s)){auto v=evaluate(x->value);if(auto n=std::dynamic_pointer_cast<ast::Variable>(x->target)){env_->set(n->name,v);if(!x->init.empty()){auto o=std::get_if<std::shared_ptr<ObjectData>>(&v.data());if(!o)throw Error(x->pos,"Indented initialization needs an object.");execute_block(x->init,std::make_shared<Environment>(env_,*o));}return;}if(auto m=std::dynamic_pointer_cast<ast::Member>(x->target)){auto owner=evaluate(m->value);auto o=std::get_if<std::shared_ptr<ObjectData>>(&owner.data());if(!o||!(*o)->fields.contains(m->name))throw Error(x->pos,"Unknown object field '"+m->name+"'.");(*o)->fields[m->name]=v;return;}if(auto i=std::dynamic_pointer_cast<ast::Index>(x->target)){auto c=evaluate(i->value),k=evaluate(i->index);if(auto l=std::get_if<std::shared_ptr<ListData>>(&c.data())){if(!std::holds_alternative<std::int64_t>(k.data()))throw Error(x->pos,"A List index needs Int.");auto n=std::get<std::int64_t>(k.data());if(n<0||static_cast<std::size_t>(n)>=(*l)->items.size())throw Error(x->pos,"List index is out of bounds.");(*l)->items[static_cast<std::size_t>(n)]=v;return;}if(auto mp=std::get_if<std::shared_ptr<MapData>>(&c.data())){if(!std::holds_alternative<std::string>(k.data()))throw Error(x->pos,"A Map key needs Text.");auto key=std::get<std::string>(k.data());for(auto&q:(*mp)->items)if(q.first==key){q.second=v;return;}(*mp)->items.emplace_back(key,v);return;}throw Error(x->pos,"Only List or Map items can be assigned by index.");}throw Error(x->pos,"You can only assign to a name, field, or collection item.");}
  if(auto x=std::dynamic_pointer_cast<ast::If>(s)){execute_block(evaluate(x->condition).truth(x->pos)?x->then_block:x->else_block,std::make_shared<Environment>(env_));return;}
  if(auto x=std::dynamic_pointer_cast<ast::Repeat>(s)){auto n=evaluate(x->count);if(!std::holds_alternative<std::int64_t>(n.data()))throw Error(x->pos,"repeat needs an Int count.");auto count=std::get<std::int64_t>(n.data());if(count<0)throw Error(x->pos,"repeat count cannot be negative.");for(std::int64_t i=0;i<count;++i)execute_block(x->body,std::make_shared<Environment>(env_));return;}
  if(auto x=std::dynamic_pointer_cast<ast::For>(s)){auto v=evaluate(x->values);if(auto l=std::get_if<std::shared_ptr<ListData>>(&v.data())){for(auto&item:(*l)->items){auto local=std::make_shared<Environment>(env_);local->define(x->names[0],item);execute_block(x->body,local);}return;}if(auto set=std::get_if<std::shared_ptr<SetData>>(&v.data())){for(auto&item:(*set)->items){auto local=std::make_shared<Environment>(env_);local->define(x->names[0],item);execute_block(x->body,local);}return;}if(auto map=std::get_if<std::shared_ptr<MapData>>(&v.data())){for(auto&item:(*map)->items){auto local=std::make_shared<Environment>(env_);local->define(x->names[0],item.first);if(x->names.size()>1)local->define(x->names[1],item.second);execute_block(x->body,local);}return;}throw Error(x->pos,"for needs a List, Set, Map, or range after 'in'.");}
  if(auto x=std::dynamic_pointer_cast<ast::While>(s)){while(evaluate(x->condition).truth(x->pos)){if(++loop_steps_>10000000)throw Error(x->pos,"This loop ran too long.","Check that its condition can become false.");execute_block(x->body,std::make_shared<Environment>(env_));}return;}
  if(auto x=std::dynamic_pointer_cast<ast::Function>(s)){auto closure=env_;auto f=callable(x->name,x->params.size(),x->params.size(),[this,closure,x](const std::vector<Value>&a,SourcePos)->Value{auto local=std::make_shared<Environment>(closure);for(std::size_t i=0;i<a.size();++i)local->define(x->params[i],a[i]);try{execute_block(x->body,local);}catch(ReturnSignal&r){return r.value;}return {};});env_->set(x->name,f);return;}
  if(auto x=std::dynamic_pointer_cast<ast::Type>(s)){auto t=std::make_shared<TypeData>();t->name=x->name;t->fields=x->fields;t->closure=env_;for(auto&m:x->methods)t->methods[m->name]=m;env_->set(x->name,t);return;}
  if(auto x=std::dynamic_pointer_cast<ast::Give>(s))throw ReturnSignal{evaluate(x->value)};
  if(auto x=std::dynamic_pointer_cast<ast::Try>(s)){try{execute_block(x->body,std::make_shared<Environment>(env_));}catch(const RuntimeFailure&e){auto local=std::make_shared<Environment>(env_);local->define(x->error_name,std::make_shared<ErrorData>(e.error()));execute_block(x->else_block,local);}return;}
  if(auto x=std::dynamic_pointer_cast<ast::Fail>(s)){auto v=evaluate(x->value);runtime_fail(x->pos,v.text(),"UserError");}
  throw Error(s->pos,"This statement is not implemented.");
}

std::shared_ptr<ModuleData> Interpreter::run_module(const ast::Module&m){
  if(m.builtin)return builtin_module(m.name);
  if(m.native)return load_native_module(m);
  auto old=env_;auto old_source=source_;auto local=std::make_shared<Environment>();env_=local;source_=m.path;install_builtins(local);
  for(auto&s:m.statements)if(auto u=std::dynamic_pointer_cast<ast::Use>(s)){auto d=modules_.find(u->name);if(d==modules_.end())throw Error(u->pos,"Module '"+u->name+"' was not loaded.");if(u->name=="file"||u->name=="path"||u->name=="time")local->define(u->name,d->second);else for(auto&[n,v]:d->second->exports){if(local->has(n))throw Error(u->pos,"The name '"+n+"' is provided by more than one module.");local->define(n,v);}}
  try{for(auto&s:m.statements)execute(s);}catch(...){env_=old;source_=old_source;throw;}
  auto out=std::make_shared<ModuleData>();out->name=m.name;for(auto&s:m.statements){std::string n;if(auto t=std::dynamic_pointer_cast<ast::Type>(s))n=t->name;else if(auto f=std::dynamic_pointer_cast<ast::Function>(s))n=f->name;else if(auto a=std::dynamic_pointer_cast<ast::Assign>(s))if(auto v=std::dynamic_pointer_cast<ast::Variable>(a->target))n=v->name;if(!n.empty()&&n[0]!='_')out->exports[n]=local->get(n,s->pos);}env_=old;source_=old_source;return out;
}

void Interpreter::run_project(const ast::Program&p){modules_.clear();for(auto&m:p.modules){auto value=run_module(m);modules_[m.name]=value;if(m.name==p.entry){env_=std::make_shared<Environment>();install_builtins(env_);for(auto&[n,v]:value->exports)env_->define(n,v);}}}
void Interpreter::run(const ast::Program& p){if(!p.modules.empty()){run_project(p);return;}for(auto&s:p.statements)execute(s);}

} // namespace s
