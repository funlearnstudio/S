#pragma once
#include "s/ast.hpp"
#include <string>
namespace s {
class CppCompiler {
public: std::string generate(const ast::Program&);
private:
  std::string expr(const ast::ExprPtr&) const;
  std::string stmt(const ast::StmtPtr&) const;
  std::string block(const ast::Block&) const;
  std::string module(const ast::Module&) const;
};
} // namespace s
