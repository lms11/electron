// Copyright (c) 2013 GitHub, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#include "shell/common/application_info.h"

#include <string>

#include "base/logging.h"
#include "electron/electron_version.h"

namespace electron {

std::string GetApplicationName() {
  // attempt #1: the string set in app.setName()
  std::string ret = OverriddenApplicationName();

  // attempt #2: Electron's name
  if (ret.empty()) {
    ret = ELECTRON_PRODUCT_NAME;
  }

  return ret;
}

std::string GetApplicationVersion() {
  std::string ret;

  // ensure ELECTRON_PRODUCT_NAME and GetApplicationVersion match up
  if (GetApplicationName() == ELECTRON_PRODUCT_NAME)
    ret = ELECTRON_VERSION_STRING;

  // try to use the string set in app.setVersion()
  if (ret.empty())
    ret = OverriddenApplicationVersion();

  // no known version number; return some safe fallback
  if (ret.empty()) {
    LOG(WARNING) << "No version found. Was app.setVersion() called?";
    ret = "0.0";
  }

  return ret;
}

}  // namespace electron