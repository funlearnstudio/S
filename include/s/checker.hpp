#pragma once
#include "s/ast.hpp"
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace s {
enum class TypeKind { Unknown, None, Int, Num, Text, Bool, Bytes, List, Map, Set, Function, Object, Module, Error, Duration, Time, Path, File, NativeHandle };
struct FunctionSig;
struct UserTypeInfo;
struct TypeInfo {
  TypeKind kind=TypeKind::Unknown;
  std::string name;
  std::shared_ptr<TypeInfo> element,key,value;
  std::shared_ptr<FunctionSig> callable;
  std::shared_ptr<UserTypeInfo> object;
  std::unordered_map<std::string,TypeInfo> members;
  TypeInfo()=default;
  explicit TypeInfo(TypeKind k);
};
struct FunctionSig { std::vector<TypeInfo> params; TypeInfo result; bool fallible=false; bool variadic=false; std::size_t min_args=0; };
struct UserTypeInfo { std::string name; std::unordered_map<std::string,TypeInfo> fields; std::unordered_map<std::string,std::shared_ptr<FunctionSig>> methods; };

inline TypeInfo::TypeInfo(TypeKind k):kind(k){
  if(k==TypeKind::Error){
    kind=TypeKind::Object;
    object=std::make_shared<UserTypeInfo>();
    object->name="Error";
    object->fields["message"]=TypeInfo(TypeKind::Text);
    object->fields["source"]=TypeInfo(TypeKind::Text);
    object->fields["line"]=TypeInfo(TypeKind::Int);
    object->fields["kind"]=TypeInfo(TypeKind::Text);
  }
}

class Checker {
public: void check(const ast::Program&);
private:
  using Scope=std::unordered_map<std::string,TypeInfo>;
  std::vector<Scope> scopes_{{}};
  std::unordered_map<std::string,Scope> module_exports_;
  std::unordered_map<std::string,std::unordered_map<std::string,std::shared_ptr<FunctionSig>>> module_callables_;
  std::shared_ptr<FunctionSig> current_function_;
  int function_depth_=0,error_depth_=0,propagate_depth_=0;
  TypeInfo expr(const ast::ExprPtr&); void stmt(const ast::StmtPtr&); void block(const ast::Block&);
  TypeInfo find(const std::string&,SourcePos) const; void put(const std::string&,TypeInfo,SourcePos);
  void constrain(const ast::ExprPtr&,const TypeInfo&);
  void check_project(const ast::Program&); void check_module(const ast::Module&);
  void install_builtins(); TypeInfo builtin_module(const std::string&) const;
  static bool compatible(const TypeInfo&,const TypeInfo&); static std::string type_text(const TypeInfo&);
  static TypeInfo from_native_name(const std::string&);
  std::shared_ptr<FunctionSig> member_call(const TypeInfo&,const std::string&,SourcePos) const;
};
} // namespace s
