#pragma once

#include "s/checker.hpp"
#include "s/value.hpp"
#include <memory>

namespace s {
class Interpreter;

void extend_database_type(TypeInfo& module);
void extend_database_module(const std::shared_ptr<ModuleData>& module, Interpreter& vm);

} // namespace s
