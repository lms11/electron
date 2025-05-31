// Copyright (c) 2022 Slack Technologies, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#include <cstdlib>

#include "base/at_exit.h"
#include "base/command_line.h"
#include "base/i18n/icu_util.h"
#include "base/strings/cstring_view.h"
#include "content/public/app/content_main.h"
#include "shell/app/electron_main_delegate.h"
#include "shell/app/node_main.h"

namespace {

[[nodiscard]] bool IsEnvSet(const base::cstring_view name) {
  const char* const indicator = getenv(name.c_str());
  return indicator && *indicator;
}

}  // namespace

// Main entry point for Android Electron app
int main(int argc, char* argv[]) {
  // Initialize base systems
  base::AtExitManager at_exit_manager;
  
  // Initialize command line
  base::CommandLine::Init(argc, argv);
  
  // Check if running as Node.js process (for separate process architecture)
  if (base::CommandLine::ForCurrentProcess()->HasSwitch("type")) {
    electron::ElectronMainDelegate delegate;
    return content::ContentMain(argc, argv, &delegate);
  }
  
  // Check if running as Node.js standalone
  if (IsEnvSet("ELECTRON_RUN_AS_NODE")) {
    return electron::NodeMain();
  }
  
  // For Android, we'll primarily be called via JNI
  // This main() function is mainly for testing and process spawning
  return 0;
}
