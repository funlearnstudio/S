#pragma once

#include "s/token.hpp"
#include <stdexcept>
#include <string>

namespace s {

class Error : public std::runtime_error {
public:
  Error(SourcePos pos, std::string message, std::string hint = {})
      : std::runtime_error(std::move(message)), pos_(pos), hint_(std::move(hint)) {}
  SourcePos pos() const { return pos_; }
  const std::string& hint() const { return hint_; }
private:
  SourcePos pos_;
  std::string hint_;
};

std::string format_error(const Error& error, const std::string& source);

} // namespace s
