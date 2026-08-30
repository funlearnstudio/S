#pragma once

#include "s/ast.hpp"
#include "s/web_compiler.hpp"

namespace s {

bool has_component_web(const ast::Program& program);

class ComponentWebCompiler {
public:
  WebBundle generate(const ast::Program& program);
};

} // namespace s