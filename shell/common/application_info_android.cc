#include "shell/common/application_info.h"

#include "base/no_destructor.h"

namespace electron {

std::string& OverriddenApplicationName() {
  static base::NoDestructor<std::string> overridden_app_name;
  return *overridden_app_name;
}

std::string& OverriddenApplicationVersion() {
  static base::NoDestructor<std::string> overridden_app_version;
  return *overridden_app_version;
}

std::string GetApplicationName() {
  const std::string& overridden_app_name = OverriddenApplicationName();
  if (!overridden_app_name.empty())
    return overridden_app_name;
  
  // Default to "Electron" for Android
  return "Electron";
}

std::string GetApplicationVersion() {
  const std::string& overridden_app_version = OverriddenApplicationVersion();
  if (!overridden_app_version.empty())
    return overridden_app_version;
  
  // Default version for Android
  return "1.0.0";
}

void SetApplicationName(const std::string& name) {
  OverriddenApplicationName() = name;
}

void SetApplicationVersion(const std::string& version) {
  OverriddenApplicationVersion() = version;
}

}  // namespace electron