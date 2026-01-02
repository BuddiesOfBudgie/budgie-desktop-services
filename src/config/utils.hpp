#pragma once

#include <filesystem>
#include <string>
#include <toml.hpp>

namespace bd::ConfigUtils {
  void                  ensureConfigPathExists(const std::filesystem::path& p);
  std::filesystem::path getConfigPath(const std::string& config_name);
}
