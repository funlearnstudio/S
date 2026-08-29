#pragma once

#include "s/token.hpp"
#include <string>
#include <vector>

namespace s {

class Lexer {
public:
  explicit Lexer(std::string source) : source_(std::move(source)) {}
  std::vector<Token> scan();
private:
  std::string source_;
};

} // namespace s
