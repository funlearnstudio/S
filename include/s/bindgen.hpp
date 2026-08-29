#pragma once

#include <filesystem>
#include <vector>

namespace s {

struct BindingOutputs {
  std::filesystem::path metadata;
  std::filesystem::path header;
  std::filesystem::path source;
};

BindingOutputs generate_bindings(const std::filesystem::path& definition,
                                 const std::filesystem::path& output_directory);

} // namespace s
