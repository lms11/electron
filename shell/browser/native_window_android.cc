// Copyright (c) 2025 GitHub, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#include "shell/browser/native_window.h"

#include <memory>

#include "shell/common/gin_helper/dictionary.h"

namespace electron {

// Android stub implementation of NativeWindow::Create
std::unique_ptr<NativeWindow> NativeWindow::Create(
    const gin_helper::Dictionary& options,
    NativeWindow* parent) {
  // TODO: Implement proper Android native window
  return nullptr;
}

}  // namespace electron