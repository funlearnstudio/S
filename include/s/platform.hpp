#pragma once

#include "s/checker.hpp"
#include "s/value.hpp"
#include "s/advanced.hpp"
#include <memory>
#include <string>

namespace s {
class Interpreter;

bool is_platform_builtin(const std::string& name);
TypeInfo platform_builtin_type(const std::string& name);
std::shared_ptr<ModuleData> platform_builtin_module(const std::string& name, Interpreter& vm);

inline bool combined_platform_builtin(const std::string& name){
  return is_platform_builtin(name)||is_advanced_builtin(name);
}

inline TypeInfo combined_platform_builtin_type(const std::string& name){
  if(is_advanced_builtin(name)) return advanced_builtin_type(name);
  auto module=platform_builtin_type(name);
  if(name=="collections") extend_collections_type(module);
  return module;
}

inline std::shared_ptr<ModuleData> combined_platform_builtin_module(const std::string& name,Interpreter& vm){
  if(is_advanced_builtin(name)) return advanced_builtin_module(name,vm);
  auto module=platform_builtin_module(name,vm);
  if(name=="collections") extend_collections_module(module,vm);
  return module;
}

} // namespace s
