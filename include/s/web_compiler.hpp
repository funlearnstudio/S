#pragma once

#include "s/ast.hpp"
#include <string>

namespace s {

struct WebBundle {
  std::string html;
  std::string css;
  std::string js;
  std::string ts;
};

class WebCompiler {
public:
  WebBundle generate(const ast::Program& program);
};

} // namespace s
