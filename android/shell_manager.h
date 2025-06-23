// Copyright 2012 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef ELECTRON_ANDROID_SHELL_MANAGER_H_
#define ELECTRON_ANDROID_SHELL_MANAGER_H_

#include <jni.h>
#include <map>

#include "base/android/jni_android.h"
#include "base/android/scoped_java_ref.h"

namespace content {

class WebContents;

// Destroys the ShellManager on app exit.
void DestroyShellManager();

}  // namespace content

#endif  // ELECTRON_ANDROID_SHELL_MANAGER_H_
