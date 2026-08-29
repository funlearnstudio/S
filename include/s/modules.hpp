#pragma once

#include "s/ast.hpp"
#include <filesystem>
#include <string>
#include <unordered_map>
#include <vector>

namespace s {
class ModuleLoader {
public:
  ast::Program load(const std::filesystem::path& entry);
private:
  std::filesystem::path root_;
  std::unordered_map<std::string,std::size_t> loaded_;
  std::vector<std::filesystem::path> stack_;
  ast::Program program_;
  std::size_t load_module(const std::string& name,const std::filesystem::path& from);
  std::filesystem::path resolve(const std::string& name,const std::filesystem::path& from,bool& builtin,bool& native) const;
  ast::Module parse_source_module(const std::string& name,const std::filesystem::path& path);
  ast::Module parse_native_module(const std::string& name,const std::filesystem::path& path);
};
} // namespace s
