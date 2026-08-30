#pragma once

#include "s/checker.hpp"
#include "s/value.hpp"
#include <memory>

namespace s {
class Interpreter;
void extend_game_type(TypeInfo& module);
void extend_game_module(const std::shared_ptr<ModuleData>& module, Interpreter& vm);
}
