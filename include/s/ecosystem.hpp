#pragma once

#include "s/checker.hpp"
#include "s/value.hpp"
#include <memory>
#include <string>

namespace s {
class Interpreter;

// Higher-level runtime modules that reuse SE's existing module/call model.
// These are kept outside the parser so adding platform capability does not
// require adding language syntax.
bool is_ecosystem_builtin(const std::string& name);
TypeInfo ecosystem_builtin_type(const std::string& name);
std::shared_ptr<ModuleData> ecosystem_builtin_module(const std::string& name, Interpreter& vm);

} // namespace s
