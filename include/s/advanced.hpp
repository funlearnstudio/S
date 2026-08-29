#pragma once

#include "s/checker.hpp"
#include "s/value.hpp"
#include <memory>
#include <string>

namespace s {
class Interpreter;

bool is_advanced_builtin(const std::string& name);
TypeInfo advanced_builtin_type(const std::string& name);
std::shared_ptr<ModuleData> advanced_builtin_module(const std::string& name, Interpreter& vm);
void extend_collections_type(TypeInfo& module);
void extend_collections_module(const std::shared_ptr<ModuleData>& module, Interpreter& vm);

} // namespace s
