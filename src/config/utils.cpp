#include "utils.hpp"

#include <QtLogging>

namespace fs = std::filesystem;

void bd::ConfigUtils::ensureConfigPathExists(const fs::path& p) {
  auto dir = p.parent_path();
  if (!fs::exists(dir)) { fs::create_directories(dir); }
}

fs::path bd::ConfigUtils::getConfigPath(const std::string& config_name) {
  const char* xdg_config_home = std::getenv("XDG_CONFIG_HOME");
  fs::path    path {};
  if (xdg_config_home) path /= xdg_config_home;
  if (xdg_config_home == nullptr) {
    const char* home = std::getenv("HOME");
    if (!home) { qFatal("HOME environment variable not set"); }
    path /= home;
    path /= ".config";
  }

  path /= "budgie-desktop";
  path /= config_name;
  return path;
}
