#pragma once
#include "s/ast.hpp"
#include <string>
#include <unordered_map>
#include <vector>

namespace s {
enum class TypeKind { Unknown, Int, Num, Text, Bool, List, Function, None };
class Checker {
public: void check(const ast::Program&);
private:
  std::vector<std::unordered_map<std::string,TypeKind>> scopes_{{}}; int function_depth_=0;
  TypeKind expr(const ast::ExprPtr&); void stmt(const ast::StmtPtr&); void block(const ast::Block&);
  TypeKind find(const std::string&,SourcePos) const; void put(const std::string&,TypeKind,SourcePos);
};
} // namespace s
