#pragma once

#include "s/ast.hpp"
#include "s/value.hpp"
#include <memory>

namespace s {
std::shared_ptr<ModuleData> load_native_module(const ast::Module& module);
} // namespace s
