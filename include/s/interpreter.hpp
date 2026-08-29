#pragma once

#include "s/ast.hpp"
#include "s/value.hpp"
#include <istream>
#include <ostream>
#include <unordered_map>

namespace s {
struct TypeData {
  std::string name;
  std::vector<ast::FieldDecl> fields;
  std::unordered_map<std::string,std::shared_ptr<ast::Function>> methods;
  std::shared_ptr<Environment> closure;
};

class Interpreter {
public:
  Interpreter(std::istream& in,std::ostream& out);
  void run(const ast::Program&); Value evaluate(const ast::ExprPtr&);
  std::shared_ptr<Environment> environment() const{return env_;}
  Value invoke(Value callable,const std::vector<Value>& args,SourcePos pos){return call(std::move(callable),args,pos);}
private:
  std::istream& in_; std::ostream& out_; std::shared_ptr<Environment> env_; int loop_steps_=0;
  std::unordered_map<std::string,std::shared_ptr<ModuleData>> modules_;
  std::string source_;
  void execute(const ast::StmtPtr&); void execute_block(const ast::Block&,std::shared_ptr<Environment>);
  void install_builtins(const std::shared_ptr<Environment>&); std::shared_ptr<ModuleData> builtin_module(const std::string&);
  void run_project(const ast::Program&); std::shared_ptr<ModuleData> run_module(const ast::Module&);
  Value instantiate(const std::shared_ptr<TypeData>&,SourcePos);
  Value member(Value,const std::string&,SourcePos,bool auto_call);
  Value call(Value,const std::vector<Value>&,SourcePos);
  [[noreturn]] void runtime_fail(SourcePos,const std::string&,const std::string& kind="Error") const;
};
} // namespace s
