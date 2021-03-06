// Copyright 2019 Intel Corporation.

#include "plaidml2/core/settings.h"

#include <boost/filesystem.hpp>

#include "base/util/env.h"
#include "plaidml2/core/internal.h"

using plaidml::core::ffi_wrap;
using plaidml::core::ffi_wrap_void;
namespace fs = boost::filesystem;

namespace plaidml {
namespace core {

namespace {

fs::path home_path() {
  auto home = vertexai::env::Get("HOME");
  if (home.size()) {
    return home;
  }
  auto user_profile = vertexai::env::Get("USERPROFILE");
  if (user_profile.size()) {
    return user_profile;
  }
  auto home_drive = vertexai::env::Get("HOMEDRIVE");
  auto home_path = vertexai::env::Get("HOMEPATH");
  if (home_drive.size() && home_path.size()) {
    return home_drive + home_path;
  }
  throw std::runtime_error("Could not detect HOME/USERPROFILE");
}

fs::path settings_path() {
  fs::path path = vertexai::env::Get("PLAIDML_SETTINGS");
  if (path.empty()) {
    path = home_path() / ".plaidml2";
  }
  return path;
}

}  // namespace

Settings::Settings() {  //
  settings_.emplace("PLAIDML_SETTINGS", settings_path().string());
}

Settings* Settings::Instance() {
  static Settings settings;
  return &settings;
}

const std::map<std::string, std::string>& Settings::all() const {  //
  return settings_;
}

std::string Settings::get(const std::string& key) const {
  auto env_var = vertexai::env::Get(key);
  if (env_var.size()) {
    return env_var;
  }
  auto it = settings_.find(key);
  if (it == settings_.end()) {
    return "";
  }
  return it->second;
}

void Settings::set(const std::string& key, const std::string& value) {
  settings_[key] = value;
  vertexai::env::Set(key, value);
}

void Settings::load() {
  fs::path settings_path{get("PLAIDML_SETTINGS")};
  if (!fs::exists(settings_path)) {
    LOG(WARNING) << "No PlaidML settings found.";
    return;
  }
  settings_.clear();
  fs::ifstream file(settings_path);
  for (std::string line; std::getline(file, line);) {
    auto pos = line.find('=');
    if (pos != std::string::npos) {
      auto key = line.substr(0, pos);
      auto value = line.substr(pos + 1);
      IVLOG(1, key << " = " << value);
      auto env_var = vertexai::env::Get(key);
      if (env_var.size()) {
        value = env_var;
      }
      set(key, value);
    }
  }
  settings_["PLAIDML_SETTINGS"] = settings_path.string();
}

void Settings::save() {
  fs::path settings_path{get("PLAIDML_SETTINGS")};
  fs::ofstream file(settings_path);
  for (const auto& kvp : settings_) {
    if (kvp.first != "PLAIDML_SETTINGS") {
      file << kvp.first << "=" << kvp.second << std::endl;
    }
  }
}

}  // namespace core
}  // namespace plaidml
