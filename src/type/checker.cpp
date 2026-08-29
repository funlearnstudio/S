#include "s/checker.hpp"
#include "s/error.hpp"
#include <algorithm>

namespace s {
namespace {
bool numeric(const TypeInfo& t){return t.kind==TypeKind::Int||t.kind==TypeKind::Num;}
TypeInfo list_of(TypeInfo e){TypeInfo t(TypeKind::List);t.element=std::make_shared<TypeInfo>(std::move(e));return t;}
TypeInfo set_of(TypeInfo e){TypeInfo t(TypeKind::Set);t.element=std::make_shared<TypeInfo>(std::move(e));return t;}
TypeInfo map_of(TypeInfo k,TypeInfo v){TypeInfo t(TypeKind::Map);t.key=std::make_shared<TypeInfo>(std::move(k));t.value=std::make_shared<TypeInfo>(std::move(v));return t;}
TypeInfo fn(std::vector<TypeInfo> p,TypeInfo r,bool fallible=false,bool variadic=false,std::size_t min=0){
  TypeInfo t(TypeKind::Function);
  t.callable=std::make_shared<FunctionSig>();
  t.callable->params=std::move(p);
  t.callable->result=std::move(r);
  t.callable->fallible=fallible;
  t.callable->variadic=variadic;
  t.callable->min_args=min;
  return t;
}
}

std::string Checker::type_text(const TypeInfo& t){
  switch(t.kind){
    case TypeKind::Unknown:return "value";
    case TypeKind::None:return "None";
    case TypeKind::Int:return "Int";
    case TypeKind::Num:return "Num";
    case TypeKind::Text:return "Text";
    case TypeKind::Bool:return "Bool";
    case TypeKind::Bytes:return "Bytes";
    case TypeKind::List:return "List";
    case TypeKind::Map:return "Map";
    case TypeKind::Set:return "Set";
    case TypeKind::Function:return "Function";
    case TypeKind::Object:return t.object?t.object->name:"Object";
    case TypeKind::Module:return "Module";
    case TypeKind::Error:return "Error";
    case TypeKind::Duration:return "Duration";
    case TypeKind::Time:return "Time";
    case TypeKind::Path:return "Path";
    case TypeKind::File:return "File";
    case TypeKind::NativeHandle:return t.name.empty()?"NativeHandle":t.name;
  }
  return "value";
}

bool Checker::compatible(const TypeInfo& a,const TypeInfo& b){
  if(a.kind==TypeKind::Unknown||b.kind==TypeKind::Unknown) return true;
  if(a.kind==b.kind){
    if(a.kind==TypeKind::Object) return !a.object||!b.object||a.object->name==b.object->name;
    if((a.kind==TypeKind::List||a.kind==TypeKind::Set)&&a.element&&b.element) return compatible(*a.element,*b.element);
    if(a.kind==TypeKind::Map&&a.key&&b.key&&a.value&&b.value) return compatible(*a.key,*b.key)&&compatible(*a.value,*b.value);
    if(a.kind==TypeKind::NativeHandle&&!a.name.empty()&&!b.name.empty()) return a.name==b.name;
    return true;
  }
  return numeric(a)&&numeric(b);
}

TypeInfo Checker::find(const std::string& n,SourcePos p) const{
  for(auto i=scopes_.rbegin();i!=scopes_.rend();++i){
    if(auto x=i->find(n);x!=i->end()) return x->second;
  }
  throw Error(p,"I don't know what \""+n+"\" is.","Create it first with "+n+" = value, or check the spelling.");
}

void Checker::put(const std::string& n,TypeInfo t,SourcePos p){
  for(auto i=scopes_.rbegin();i!=scopes_.rend();++i){
    if(auto x=i->find(n);x!=i->end()){
      if(!compatible(x->second,t)) throw Error(p,"'"+n+"' needs "+type_text(x->second)+", but you gave it "+type_text(t)+".");
      if(x->second.kind==TypeKind::Unknown&&t.kind!=TypeKind::Unknown) x->second=std::move(t);
      return;
    }
  }
  scopes_.back()[n]=std::move(t);
}

void Checker::constrain(const ast::ExprPtr& e,const TypeInfo& t){
  if(auto v=std::dynamic_pointer_cast<ast::Variable>(e)) put(v->name,t,v->pos);
}

void Checker::install_builtins(){
  auto& g=scopes_.front();
  if(g.contains("read")) return;
  TypeInfo pathish(TypeKind::Unknown);
  g["read"]=fn({pathish},TypeInfo(TypeKind::Text),true);
  g["write"]=fn({pathish,TypeInfo(TypeKind::Text)},TypeInfo(TypeKind::None),true);
  g["append"]=fn({pathish,TypeInfo(TypeKind::Text)},TypeInfo(TypeKind::None),true);
  g["open"]=fn({pathish},TypeInfo(TypeKind::File),true);
  g["wait"]=fn({TypeInfo(TypeKind::Duration)},TypeInfo(TypeKind::None));
  g["bytes"]=fn({TypeInfo(TypeKind::Text)},TypeInfo(TypeKind::Bytes));
}

TypeInfo Checker::builtin_module(const std::string& name) const{
  TypeInfo m(TypeKind::Module);
  m.name=name;
  if(name=="path"){
    m.members["join"]=fn({TypeInfo(TypeKind::Unknown)},TypeInfo(TypeKind::Path),false,true,1);
    m.members["name"]=fn({TypeInfo(TypeKind::Unknown)},TypeInfo(TypeKind::Text));
    m.members["ext"]=fn({TypeInfo(TypeKind::Unknown)},TypeInfo(TypeKind::Text));
    m.members["parent"]=fn({TypeInfo(TypeKind::Unknown)},TypeInfo(TypeKind::Path));
    m.members["exists"]=fn({TypeInfo(TypeKind::Unknown)},TypeInfo(TypeKind::Bool));
    m.members["is_file"]=fn({TypeInfo(TypeKind::Unknown)},TypeInfo(TypeKind::Bool));
    m.members["is_dir"]=fn({TypeInfo(TypeKind::Unknown)},TypeInfo(TypeKind::Bool));
  }else if(name=="time"){
    m.members["now"]=fn({},TypeInfo(TypeKind::Time));
  }else if(name=="file"){
    m.members["read"]=fn({TypeInfo(TypeKind::Unknown)},TypeInfo(TypeKind::Text),true);
    m.members["write"]=fn({TypeInfo(TypeKind::Unknown),TypeInfo(TypeKind::Text)},TypeInfo(TypeKind::None),true);
    m.members["append"]=fn({TypeInfo(TypeKind::Unknown),TypeInfo(TypeKind::Text)},TypeInfo(TypeKind::None),true);
    m.members["open"]=fn({TypeInfo(TypeKind::Unknown)},TypeInfo(TypeKind::File),true);
  }
  return m;
}

TypeInfo Checker::from_native_name(const std::string& n){
  if(n=="Int") return TypeInfo(TypeKind::Int);
  if(n=="Num") return TypeInfo(TypeKind::Num);
  if(n=="Bool") return TypeInfo(TypeKind::Bool);
  if(n=="Text") return TypeInfo(TypeKind::Text);
  if(n=="Bytes") return TypeInfo(TypeKind::Bytes);
  if(n=="None") return TypeInfo(TypeKind::None);
  if(n.rfind("Handle:",0)==0){TypeInfo t(TypeKind::NativeHandle);t.name=n.substr(7);return t;}
  return TypeInfo(TypeKind::Unknown);
}

std::shared_ptr<FunctionSig> Checker::member_call(const TypeInfo& t,const std::string& name,SourcePos p) const{
  if(t.kind==TypeKind::Object&&t.object){
    if(auto i=t.object->methods.find(name);i!=t.object->methods.end()) return i->second;
  }
  if(t.kind==TypeKind::Module){
    if(auto i=t.members.find(name);i!=t.members.end()&&i->second.callable) return i->second.callable;
  }
  if(t.kind==TypeKind::List||t.kind==TypeKind::Set){
    TypeInfo elem=t.element?*t.element:TypeInfo{};
    if(name=="add") return fn({elem},TypeInfo(TypeKind::None)).callable;
    if(name=="remove") return fn({elem},TypeInfo(TypeKind::Bool)).callable;
  }
  if(t.kind==TypeKind::File){
    if(name=="read") return fn({},TypeInfo(TypeKind::Text),true).callable;
    if(name=="close") return fn({},TypeInfo(TypeKind::None)).callable;
    if(name=="write") return fn({TypeInfo(TypeKind::Text)},TypeInfo(TypeKind::None),true).callable;
  }
  throw Error(p,type_text(t)+" has no method named '"+name+"'.","Use value.help to see the members available on this value.");
}

TypeInfo Checker::expr(const ast::ExprPtr& e){
  if(auto x=std::dynamic_pointer_cast<ast::Literal>(e)){
    switch(x->value.index()){
      case 0:return TypeInfo(TypeKind::Int);
      case 1:return TypeInfo(TypeKind::Num);
      case 2:return TypeInfo(TypeKind::Text);
      default:return TypeInfo(TypeKind::Bool);
    }
  }
  if(std::dynamic_pointer_cast<ast::Duration>(e)) return TypeInfo(TypeKind::Duration);
  if(auto x=std::dynamic_pointer_cast<ast::Variable>(e)) return find(x->name,x->pos);
  if(auto x=std::dynamic_pointer_cast<ast::List>(e)){
    TypeInfo elem;
    for(auto& i:x->items){
      auto t=expr(i);
      if(elem.kind==TypeKind::Unknown) elem=t;
      else if(!compatible(elem,t)) throw Error(i->pos,"A List keeps one value type. It already has "+type_text(elem)+", but this is "+type_text(t)+".");
    }
    return list_of(elem);
  }
  if(auto x=std::dynamic_pointer_cast<ast::Set>(e)){
    TypeInfo elem;
    for(auto& i:x->items){
      auto t=expr(i);
      if(elem.kind==TypeKind::Unknown) elem=t;
      else if(!compatible(elem,t)) throw Error(i->pos,"A Set keeps one value type. It already has "+type_text(elem)+", but this is "+type_text(t)+".");
    }
    return set_of(elem);
  }
  if(auto x=std::dynamic_pointer_cast<ast::Map>(e)){
    TypeInfo value;
    for(auto& i:x->items){
      auto k=expr(i.first);
      if(k.kind!=TypeKind::Text) throw Error(i.first->pos,"Map keys are Text in SE.");
      auto v=expr(i.second);
      if(value.kind==TypeKind::Unknown) value=v;
      else if(!compatible(value,v)) throw Error(i.second->pos,"A Map keeps one value type. It already has "+type_text(value)+", but this is "+type_text(v)+".");
    }
    return map_of(TypeInfo(TypeKind::Text),value);
  }
  if(auto x=std::dynamic_pointer_cast<ast::Ask>(e)){
    if(expr(x->question).kind!=TypeKind::Text) throw Error(x->pos,"ask needs Text.");
    return TypeInfo(TypeKind::Text);
  }
  if(auto x=std::dynamic_pointer_cast<ast::TryExpr>(e)){
    ++propagate_depth_;
    auto t=expr(x->value);
    --propagate_depth_;
    if(current_function_) current_function_->fallible=true;
    return t;
  }
  if(auto x=std::dynamic_pointer_cast<ast::Range>(e)){
    if(expr(x->start).kind!=TypeKind::Int||expr(x->end).kind!=TypeKind::Int) throw Error(x->pos,"A range needs two Int values.");
    return list_of(TypeInfo(TypeKind::Int));
  }
  if(auto x=std::dynamic_pointer_cast<ast::Index>(e)){
    auto v=expr(x->value);
    auto i=expr(x->index);
    if(v.kind==TypeKind::List){if(i.kind!=TypeKind::Int) throw Error(x->pos,"A List index needs Int.");return v.element?*v.element:TypeInfo{};}
    if(v.kind==TypeKind::Map){if(i.kind!=TypeKind::Text) throw Error(x->pos,"A Map key needs Text.");return v.value?*v.value:TypeInfo{};}
    throw Error(x->pos,"Only List and Map can use [index].");
  }
  if(auto x=std::dynamic_pointer_cast<ast::Member>(e)){
    auto t=expr(x->value);
    if(x->name=="help") return TypeInfo(TypeKind::Text);
    if(x->name=="len"&&(t.kind==TypeKind::Text||t.kind==TypeKind::Bytes||t.kind==TypeKind::List||t.kind==TypeKind::Map||t.kind==TypeKind::Set)) return TypeInfo(TypeKind::Int);
    if((x->name=="upper"||x->name=="lower")&&t.kind==TypeKind::Text) return TypeInfo(TypeKind::Text);
    if(t.kind==TypeKind::Object&&t.object){
      if(auto f=t.object->fields.find(x->name);f!=t.object->fields.end()) return f->second;
      if(auto m=t.object->methods.find(x->name);m!=t.object->methods.end()){if(m->second->params.empty()) return m->second->result;TypeInfo r(TypeKind::Function);r.callable=m->second;return r;}
    }
    if(t.kind==TypeKind::Module){if(auto m=t.members.find(x->name);m!=t.members.end()){if(m->second.callable&&m->second.callable->params.empty()) return m->second.callable->result;return m->second;}}
    if(t.kind==TypeKind::File){auto sig=member_call(t,x->name,x->pos);if(sig->params.empty()) return sig->result;TypeInfo r(TypeKind::Function);r.callable=sig;return r;}
    throw Error(x->pos,type_text(t)+" has no member named '"+x->name+"'.","Use value.help to see the members available on this value.");
  }
  if(auto x=std::dynamic_pointer_cast<ast::Unary>(e)){auto t=expr(x->value);if(x->op==TokenKind::Not&&t.kind==TypeKind::Bool) return TypeInfo(TypeKind::Bool);if(x->op==TokenKind::Minus&&numeric(t)) return t;throw Error(x->pos,"This unary operation does not work with "+type_text(t)+".");}
  if(auto x=std::dynamic_pointer_cast<ast::Binary>(e)){
    auto a=expr(x->left);auto b=expr(x->right);
    if(x->op==TokenKind::And||x->op==TokenKind::Or){if(a.kind!=TypeKind::Unknown&&a.kind!=TypeKind::Bool)throw Error(x->pos,"'and' and 'or' need Bool values.");if(b.kind!=TypeKind::Unknown&&b.kind!=TypeKind::Bool)throw Error(x->pos,"'and' and 'or' need Bool values.");return TypeInfo(TypeKind::Bool);}
    if(x->op==TokenKind::EqualEqual||x->op==TokenKind::BangEqual||x->op==TokenKind::In) return TypeInfo(TypeKind::Bool);
    if(x->op==TokenKind::Greater||x->op==TokenKind::Less||x->op==TokenKind::GreaterEqual||x->op==TokenKind::LessEqual){if(a.kind==TypeKind::Unknown&&numeric(b))constrain(x->left,b);if(b.kind==TypeKind::Unknown&&numeric(a))constrain(x->right,a);if((a.kind!=TypeKind::Unknown&&!numeric(a))||(b.kind!=TypeKind::Unknown&&!numeric(b)))throw Error(x->pos,"Comparisons need numbers.");return TypeInfo(TypeKind::Bool);}
    if(x->op==TokenKind::Plus&&a.kind==TypeKind::Text&&b.kind==TypeKind::Text) return TypeInfo(TypeKind::Text);
    if(a.kind==TypeKind::Unknown&&numeric(b)){constrain(x->left,b);a=b;}if(b.kind==TypeKind::Unknown&&numeric(a)){constrain(x->right,a);b=a;}
    if(a.kind==TypeKind::Unknown||b.kind==TypeKind::Unknown) return TypeInfo{};
    if(!numeric(a)||!numeric(b)) throw Error(x->pos,"This math operation cannot combine "+type_text(a)+" and "+type_text(b)+".");
    return (a.kind==TypeKind::Num||b.kind==TypeKind::Num||x->op==TokenKind::Slash||x->op==TokenKind::Power)?TypeInfo(TypeKind::Num):TypeInfo(TypeKind::Int);
  }
  if(auto x=std::dynamic_pointer_cast<ast::Call>(e)){
    std::shared_ptr<FunctionSig> sig;
    if(auto m=std::dynamic_pointer_cast<ast::Member>(x->callee)) sig=member_call(expr(m->value),m->name,m->pos);
    else{auto c=expr(x->callee);if(c.kind!=TypeKind::Function||!c.callable)throw Error(x->pos,"Only a function or method can be called.");sig=c.callable;}
    if((!sig->variadic&&x->args.size()!=sig->params.size())||(sig->variadic&&x->args.size()<sig->min_args)) throw Error(x->pos,"This call needs "+std::to_string(sig->variadic?sig->min_args:sig->params.size())+(sig->variadic?" or more":"")+" values, but got "+std::to_string(x->args.size())+".");
    for(std::size_t i=0;i<x->args.size();++i){auto got=expr(x->args[i]);if(!sig->variadic&&i<sig->params.size()&&!compatible(sig->params[i],got))throw Error(x->args[i]->pos,"This value needs "+type_text(sig->params[i])+", but you gave it "+type_text(got)+".");}
    if(sig->fallible&&error_depth_==0&&propagate_depth_==0) throw Error(x->pos,"This operation can fail.","Handle it with a try block, or propagate it with 'try '+expression.");
    if(sig->fallible&&propagate_depth_>0&&current_function_) current_function_->fallible=true;
    return sig->result;
  }
  return TypeInfo{};
}

void Checker::block(const ast::Block& b){scopes_.push_back({});for(auto& s:b)stmt(s);scopes_.pop_back();}

void Checker::stmt(const ast::StmtPtr& s){
  if(std::dynamic_pointer_cast<ast::Use>(s)) return;
  if(auto x=std::dynamic_pointer_cast<ast::Say>(s)){expr(x->value);return;}
  if(auto x=std::dynamic_pointer_cast<ast::ExprStmt>(s)){expr(x->value);return;}
  if(auto x=std::dynamic_pointer_cast<ast::Assign>(s)){
    auto t=expr(x->value);
    if(auto n=std::dynamic_pointer_cast<ast::Variable>(x->target)){put(n->name,t,x->pos);if(!x->init.empty()){if(t.kind!=TypeKind::Object||!t.object)throw Error(x->pos,"Indented initialization needs a user-defined type.");for(auto& i:x->init){auto a=std::dynamic_pointer_cast<ast::Assign>(i);auto f=a?std::dynamic_pointer_cast<ast::Variable>(a->target):nullptr;if(!a||!f)throw Error(i->pos,"Object initialization only contains field assignments.");auto field=t.object->fields.find(f->name);if(field==t.object->fields.end())throw Error(f->pos,"'"+t.object->name+"' has no field named '"+f->name+"'.");auto got=expr(a->value);if(!compatible(field->second,got))throw Error(a->pos,"'"+f->name+"' needs "+type_text(field->second)+", but you gave it "+type_text(got)+".");}}return;}
    if(auto m=std::dynamic_pointer_cast<ast::Member>(x->target)){auto o=expr(m->value);if(o.kind!=TypeKind::Object||!o.object)throw Error(x->pos,"Only an object field can be assigned with '.'.");auto f=o.object->fields.find(m->name);if(f==o.object->fields.end())throw Error(m->pos,"'"+o.object->name+"' has no field named '"+m->name+"'.");if(!compatible(f->second,t))throw Error(x->pos,"'"+m->name+"' needs "+type_text(f->second)+", but you gave it "+type_text(t)+".");return;}
    if(auto i=std::dynamic_pointer_cast<ast::Index>(x->target)){auto c=expr(i->value);auto k=expr(i->index);if(c.kind==TypeKind::List){if(k.kind!=TypeKind::Int)throw Error(i->pos,"A List index needs Int.");if(c.element&&!compatible(*c.element,t))throw Error(x->pos,"This List needs "+type_text(*c.element)+" values.");return;}if(c.kind==TypeKind::Map){if(k.kind!=TypeKind::Text)throw Error(i->pos,"A Map key needs Text.");if(c.value&&!compatible(*c.value,t))throw Error(x->pos,"This Map needs "+type_text(*c.value)+" values.");return;}throw Error(x->pos,"Only List or Map items can be assigned by index.");}
    throw Error(x->pos,"You can only assign to a name, field, or collection item.");
  }
  if(auto x=std::dynamic_pointer_cast<ast::If>(s)){auto t=expr(x->condition);if(t.kind!=TypeKind::Bool&&t.kind!=TypeKind::Unknown)throw Error(x->pos,"if needs a Bool condition.");block(x->then_block);block(x->else_block);return;}
  if(auto x=std::dynamic_pointer_cast<ast::Repeat>(s)){if(expr(x->count).kind!=TypeKind::Int)throw Error(x->pos,"repeat needs an Int count.");block(x->body);return;}
  if(auto x=std::dynamic_pointer_cast<ast::For>(s)){auto c=expr(x->values);scopes_.push_back({});if(c.kind==TypeKind::Map){if(x->names.size()>2)throw Error(x->pos,"A Map loop uses at most key and value.");scopes_.back()[x->names[0]]=TypeInfo(TypeKind::Text);if(x->names.size()==2)scopes_.back()[x->names[1]]=c.value?*c.value:TypeInfo{};}else if(c.kind==TypeKind::List||c.kind==TypeKind::Set){if(x->names.size()!=1)throw Error(x->pos,"List and Set loops use one loop name.");scopes_.back()[x->names[0]]=c.element?*c.element:TypeInfo{};}else throw Error(x->pos,"for needs a List, Set, Map, or range after 'in'.");for(auto& i:x->body)stmt(i);scopes_.pop_back();return;}
  if(auto x=std::dynamic_pointer_cast<ast::While>(s)){auto t=expr(x->condition);if(t.kind!=TypeKind::Bool&&t.kind!=TypeKind::Unknown)throw Error(x->pos,"while needs a Bool condition.");block(x->body);return;}
  if(auto x=std::dynamic_pointer_cast<ast::Function>(s)){auto f=find(x->name,x->pos);auto sig=f.callable;if(!sig){sig=std::make_shared<FunctionSig>();sig->params.resize(x->params.size());}scopes_.push_back({});for(std::size_t i=0;i<x->params.size();++i)scopes_.back()[x->params[i]]=i<sig->params.size()?sig->params[i]:TypeInfo{};auto old=current_function_;current_function_=sig;++function_depth_;for(auto& i:x->body)stmt(i);--function_depth_;for(std::size_t i=0;i<x->params.size();++i)sig->params[i]=scopes_.back()[x->params[i]];if(sig->result.kind==TypeKind::Unknown)sig->result=TypeInfo(TypeKind::None);current_function_=old;scopes_.pop_back();return;}
  if(auto x=std::dynamic_pointer_cast<ast::Type>(s)){auto type=find(x->name,x->pos);auto info=type.object;if(!info)throw Error(x->pos,"Internal type registration error.");for(auto& f:x->fields)info->fields[f.name]=expr(f.value);for(auto& m:x->methods)if(!info->methods.contains(m->name)){auto sig=std::make_shared<FunctionSig>();sig->params.resize(m->params.size());info->methods[m->name]=sig;}for(auto& m:x->methods){auto sig=info->methods[m->name];scopes_.push_back(info->fields);for(std::size_t i=0;i<m->params.size();++i)scopes_.back()[m->params[i]]=sig->params[i];auto old=current_function_;current_function_=sig;++function_depth_;for(auto& i:m->body)stmt(i);--function_depth_;for(std::size_t i=0;i<m->params.size();++i)sig->params[i]=scopes_.back()[m->params[i]];for(auto& f:info->fields)if(auto q=scopes_.back().find(f.first);q!=scopes_.back().end())f.second=q->second;if(sig->result.kind==TypeKind::Unknown)sig->result=TypeInfo(TypeKind::None);current_function_=old;scopes_.pop_back();}return;}
  if(auto x=std::dynamic_pointer_cast<ast::Give>(s)){if(!function_depth_||!current_function_)throw Error(x->pos,"give can only be used inside a function or method.");auto t=expr(x->value);if(current_function_->result.kind==TypeKind::Unknown)current_function_->result=t;else if(!compatible(current_function_->result,t))throw Error(x->pos,"This function gives different value types.");return;}
  if(auto x=std::dynamic_pointer_cast<ast::Try>(s)){++error_depth_;block(x->body);--error_depth_;scopes_.push_back({{x->error_name,TypeInfo(TypeKind::Error)}});for(auto& i:x->else_block)stmt(i);scopes_.pop_back();return;}
  if(auto x=std::dynamic_pointer_cast<ast::Fail>(s)){if(expr(x->value).kind!=TypeKind::Text)throw Error(x->pos,"fail needs Text.");if(current_function_)current_function_->fallible=true;return;}
}

void Checker::check_module(const ast::Module& m){
  scopes_.clear();scopes_.push_back({});install_builtins();
  if(m.builtin){module_exports_[m.name]={{m.name,builtin_module(m.name)}};return;}
  if(m.native){Scope exports;for(auto& f:m.native_functions){std::vector<TypeInfo> args;for(auto& a:f.args)args.push_back(from_native_name(a));exports[f.name]=fn(std::move(args),from_native_name(f.result),f.fallible);}module_exports_[m.name]=std::move(exports);return;}
  for(auto& s:m.statements)if(auto u=std::dynamic_pointer_cast<ast::Use>(s)){auto found=module_exports_.find(u->name);if(found==module_exports_.end())throw Error(u->pos,"Module '"+u->name+"' was not loaded.");for(auto& [name,t]:found->second){if(scopes_.back().contains(name))throw Error(u->pos,"The name '"+name+"' is provided by more than one module.");scopes_.back()[name]=t;}}
  for(auto& s:m.statements){if(auto t=std::dynamic_pointer_cast<ast::Type>(s)){if(scopes_.back().contains(t->name))throw Error(t->pos,"The name '"+t->name+"' already exists.");auto info=std::make_shared<UserTypeInfo>();info->name=t->name;TypeInfo object(TypeKind::Object);object.object=info;scopes_.back()[t->name]=object;}else if(auto f=std::dynamic_pointer_cast<ast::Function>(s)){if(scopes_.back().contains(f->name))throw Error(f->pos,"The name '"+f->name+"' already exists.");auto sig=std::make_shared<FunctionSig>();sig->params.resize(f->params.size());TypeInfo t(TypeKind::Function);t.callable=sig;scopes_.back()[f->name]=t;}}
  for(auto& s:m.statements)stmt(s);
  Scope exports;for(auto& s:m.statements){std::string name;if(auto t=std::dynamic_pointer_cast<ast::Type>(s))name=t->name;else if(auto f=std::dynamic_pointer_cast<ast::Function>(s))name=f->name;else if(auto a=std::dynamic_pointer_cast<ast::Assign>(s))if(auto v=std::dynamic_pointer_cast<ast::Variable>(a->target))name=v->name;if(!name.empty()&&name[0]!='_')exports[name]=find(name,s->pos);}module_exports_[m.name]=std::move(exports);
}
void Checker::check_project(const ast::Program& p){module_exports_.clear();for(auto& m:p.modules)check_module(m);}
void Checker::check(const ast::Program& p){if(!p.modules.empty()){check_project(p);return;}if(scopes_.empty())scopes_.push_back({});install_builtins();for(auto& s:p.statements){if(auto t=std::dynamic_pointer_cast<ast::Type>(s)){if(!scopes_.back().contains(t->name)){auto info=std::make_shared<UserTypeInfo>();info->name=t->name;TypeInfo obj(TypeKind::Object);obj.object=info;scopes_.back()[t->name]=obj;}}else if(auto f=std::dynamic_pointer_cast<ast::Function>(s)){if(!scopes_.back().contains(f->name)){auto sig=std::make_shared<FunctionSig>();sig->params.resize(f->params.size());TypeInfo ft(TypeKind::Function);ft.callable=sig;scopes_.back()[f->name]=ft;}}}for(auto& s:p.statements)stmt(s);}

} // namespace s
