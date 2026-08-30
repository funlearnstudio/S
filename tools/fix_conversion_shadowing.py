from pathlib import Path


def replace_once(path: str, old: str, new: str) -> None:
    p = Path(path)
    text = p.read_text()
    if old not in text:
        raise SystemExit(f"expected text not found in {path}: {old[:120]!r}")
    if text.count(old) != 1:
        raise SystemExit(f"expected one match in {path}, found {text.count(old)}")
    p.write_text(text.replace(old, new, 1))

# Conversion names are fallback intrinsics, not reserved global variables.
# This preserves existing SE code such as `text = ...`, `use text`, and `make double n`.
interpreter_registrations = '''  e->define("text",callable("text",1,1,[](const std::vector<Value>&a,SourcePos){return Value(a[0].text());}));
  e->define("string",callable("string",1,1,[](const std::vector<Value>&a,SourcePos){return Value(a[0].text());}));
  e->define("int",callable("int",1,1,[](const std::vector<Value>&a,SourcePos p){return convert_int_value(a[0],p);}));
  e->define("integer",callable("integer",1,1,[](const std::vector<Value>&a,SourcePos p){return convert_int_value(a[0],p);}));
  e->define("num",callable("num",1,1,[](const std::vector<Value>&a,SourcePos p){return convert_num_value(a[0],p);}));
  e->define("double",callable("double",1,1,[](const std::vector<Value>&a,SourcePos p){return convert_num_value(a[0],p);}));
  e->define("float",callable("float",1,1,[](const std::vector<Value>&a,SourcePos p){return convert_num_value(a[0],p);}));
  e->define("bool",callable("bool",1,1,[](const std::vector<Value>&a,SourcePos p){return convert_bool_value(a[0],p);}));
  e->define("boolean",callable("boolean",1,1,[](const std::vector<Value>&a,SourcePos p){return convert_bool_value(a[0],p);}));
  e->define("char",callable("char",1,1,[](const std::vector<Value>&a,SourcePos p){return convert_char_value(a[0],p);}));
'''
replace_once("src/interpreter/interpreter.cpp", interpreter_registrations, "")

char_anchor = '''Value convert_char_value(const Value& value,SourcePos p){
  if(auto text=std::get_if<std::string>(&value.data())){
    if(!one_utf8_character(*text))throw Error(p,"char needs exactly one Unicode character.");
    return Value(*text);
  }
  if(auto code=std::get_if<std::int64_t>(&value.data()))return Value(utf8_from_codepoint(*code,p));
  throw Error(p,"char needs one-character Text or an Int Unicode code point.");
}
'''
char_with_fallback = char_anchor + '''std::shared_ptr<CallableData> conversion_callable(const std::string& name){
  if(name=="text"||name=="string")return callable(name,1,1,[](const std::vector<Value>&a,SourcePos){return Value(a[0].text());});
  if(name=="int"||name=="integer")return callable(name,1,1,[](const std::vector<Value>&a,SourcePos p){return convert_int_value(a[0],p);});
  if(name=="num"||name=="double"||name=="float")return callable(name,1,1,[](const std::vector<Value>&a,SourcePos p){return convert_num_value(a[0],p);});
  if(name=="bool"||name=="boolean")return callable(name,1,1,[](const std::vector<Value>&a,SourcePos p){return convert_bool_value(a[0],p);});
  if(name=="char")return callable(name,1,1,[](const std::vector<Value>&a,SourcePos p){return convert_char_value(a[0],p);});
  return {};
}
'''
replace_once("src/interpreter/interpreter.cpp", char_anchor, char_with_fallback)

var_old = '''if(auto x=std::dynamic_pointer_cast<ast::Variable>(e)){auto v=env_->get(x->name,x->pos);if(auto t=std::get_if<std::shared_ptr<TypeData>>(&v.data()))return instantiate(*t,x->pos);if(auto f=std::get_if<std::shared_ptr<CallableData>>(&v.data());f&&(*f)->min_args==0&&!(*f)->variadic)return call(v,{},x->pos);return v;}'''
var_new = '''if(auto x=std::dynamic_pointer_cast<ast::Variable>(e)){Value v;if(env_->has(x->name))v=env_->get(x->name,x->pos);else if(auto builtin=conversion_callable(x->name))v=Value(builtin);else v=env_->get(x->name,x->pos);if(auto t=std::get_if<std::shared_ptr<TypeData>>(&v.data()))return instantiate(*t,x->pos);if(auto f=std::get_if<std::shared_ptr<CallableData>>(&v.data());f&&(*f)->min_args==0&&!(*f)->variadic)return call(v,{},x->pos);return v;}'''
replace_once("src/interpreter/interpreter.cpp", var_old, var_new)

call_old = '''if(auto x=std::dynamic_pointer_cast<ast::Call>(e)){Value c;if(auto m=std::dynamic_pointer_cast<ast::Member>(x->callee))c=member(evaluate(m->value),m->name,m->pos,false);else c=env_->get(std::dynamic_pointer_cast<ast::Variable>(x->callee)?std::dynamic_pointer_cast<ast::Variable>(x->callee)->name:"",x->pos);if(!std::dynamic_pointer_cast<ast::Variable>(x->callee)&&!std::dynamic_pointer_cast<ast::Member>(x->callee))c=evaluate(x->callee);std::vector<Value>a;for(auto&i:x->args)a.push_back(evaluate(i));return call(c,a,x->pos);}'''
call_new = '''if(auto x=std::dynamic_pointer_cast<ast::Call>(e)){Value c;if(auto m=std::dynamic_pointer_cast<ast::Member>(x->callee))c=member(evaluate(m->value),m->name,m->pos,false);else if(auto v=std::dynamic_pointer_cast<ast::Variable>(x->callee)){if(env_->has(v->name))c=env_->get(v->name,x->pos);else if(auto builtin=conversion_callable(v->name))c=Value(builtin);else c=env_->get(v->name,x->pos);}else c=evaluate(x->callee);std::vector<Value>a;for(auto&i:x->args)a.push_back(evaluate(i));return call(c,a,x->pos);}'''
replace_once("src/interpreter/interpreter.cpp", call_old, call_new)

checker_registrations = '''  g["text"]=fn({TypeInfo(TypeKind::Unknown)},TypeInfo(TypeKind::Text));
  g["string"]=fn({TypeInfo(TypeKind::Unknown)},TypeInfo(TypeKind::Text));
  g["int"]=fn({TypeInfo(TypeKind::Unknown)},TypeInfo(TypeKind::Int));
  g["integer"]=fn({TypeInfo(TypeKind::Unknown)},TypeInfo(TypeKind::Int));
  g["num"]=fn({TypeInfo(TypeKind::Unknown)},TypeInfo(TypeKind::Num));
  g["double"]=fn({TypeInfo(TypeKind::Unknown)},TypeInfo(TypeKind::Num));
  g["float"]=fn({TypeInfo(TypeKind::Unknown)},TypeInfo(TypeKind::Num));
  g["bool"]=fn({TypeInfo(TypeKind::Unknown)},TypeInfo(TypeKind::Bool));
  g["boolean"]=fn({TypeInfo(TypeKind::Unknown)},TypeInfo(TypeKind::Bool));
  g["char"]=fn({TypeInfo(TypeKind::Unknown)},TypeInfo(TypeKind::Text));
'''
replace_once("src/type/checker.cpp", checker_registrations, "")

substitute_anchor = '''TypeInfo substitute(const TypeInfo& t,const std::unordered_map<std::string,TypeInfo>& bindings){
  if(t.kind==TypeKind::Generic){auto i=bindings.find(t.name);return i==bindings.end()?t:i->second;}
  TypeInfo out=t;
  if(t.element)out.element=std::make_shared<TypeInfo>(substitute(*t.element,bindings));
  if(t.key)out.key=std::make_shared<TypeInfo>(substitute(*t.key,bindings));
  if(t.value)out.value=std::make_shared<TypeInfo>(substitute(*t.value,bindings));
  return out;
}
'''
substitute_with_conversion = substitute_anchor + '''bool conversion_builtin_type(const std::string& name,TypeInfo& out){
  if(name=="text"||name=="string"){out=fn({TypeInfo(TypeKind::Unknown)},TypeInfo(TypeKind::Text));return true;}
  if(name=="int"||name=="integer"){out=fn({TypeInfo(TypeKind::Unknown)},TypeInfo(TypeKind::Int));return true;}
  if(name=="num"||name=="double"||name=="float"){out=fn({TypeInfo(TypeKind::Unknown)},TypeInfo(TypeKind::Num));return true;}
  if(name=="bool"||name=="boolean"){out=fn({TypeInfo(TypeKind::Unknown)},TypeInfo(TypeKind::Bool));return true;}
  if(name=="char"){out=fn({TypeInfo(TypeKind::Unknown)},TypeInfo(TypeKind::Text));return true;}
  return false;
}
'''
replace_once("src/type/checker.cpp", substitute_anchor, substitute_with_conversion)

find_old = '''TypeInfo Checker::find(const std::string& n,SourcePos p) const{
  for(auto i=scopes_.rbegin();i!=scopes_.rend();++i){if(auto x=i->find(n);x!=i->end())return x->second;}
  throw Error(p,"I don't know what \\\""+n+"\\\" is.","Create it first with "+n+" = value, or check the spelling.");
}
'''
find_new = '''TypeInfo Checker::find(const std::string& n,SourcePos p) const{
  for(auto i=scopes_.rbegin();i!=scopes_.rend();++i){if(auto x=i->find(n);x!=i->end())return x->second;}
  TypeInfo conversion;if(conversion_builtin_type(n,conversion))return conversion;
  throw Error(p,"I don't know what \\\""+n+"\\\" is.","Create it first with "+n+" = value, or check the spelling.");
}
'''
replace_once("src/type/checker.cpp", find_old, find_new)

print("conversion shadowing fixed")
