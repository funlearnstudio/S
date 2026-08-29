#pragma once
#include "s/ast.hpp"
#include <string>
namespace s {
class CppCompiler {
public: std::string generate(const ast::Program&);
private:
  int id_=0,indent_=1; std::string out_; std::string env_="env";
  std::string expr(const ast::ExprPtr&); void stmt(const ast::StmtPtr&); void block(const ast::Block&);
  std::string pad() const{return std::string(indent_*2,' ');} std::string fresh(){return "_s"+std::to_string(id_++);}
};
} // namespace s
