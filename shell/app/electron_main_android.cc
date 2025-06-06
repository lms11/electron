// Copyright (c) 2024 The Electron Authors. All rights reserved.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#include <jni.h>
#include <stdlib.h>

#include "base/android/base_jni_onload.h"
#include "base/android/jni_android.h"
#include "base/android/jni_utils.h"
#include "base/android/library_loader/library_loader_hooks.h"
#include "content/public/app/content_jni_onload.h"
#include "content/public/app/content_main.h"
#include "content/public/browser/android/compositor.h"
#include "electron/buildflags/buildflags.h"
#include "electron/shell/app/electron_main_delegate.h"
#include "electron/shell/browser/electron_browser_main_parts.h"

namespace electron {

// Forward declaration
namespace android {
bool RegisterElectronJni(JNIEnv* env);
}  // namespace android

namespace {

bool NativeInit(base::android::LibraryProcessType library_process_type) {
  // Initialize the command line.
  // TODO: Initialize Electron's main delegate when ready
  
  return true;
}

}  // namespace

bool OnJNIOnLoadInit() {
  if (!base::android::OnJNIOnLoadInit())
    return false;

  // Set up JNI bindings for content.
  if (!content::android::OnJNIOnLoadInit())
    return false;

  // Register Electron-specific JNI bindings
  JNIEnv* env = base::android::AttachCurrentThread();
  if (!android::RegisterElectronJni(env)) {
    LOG(ERROR) << "Failed to register Electron JNI";
    return false;
  }

  return true;
}

void InitializeElectron() {
  // This is called from Java after the browser process is started.
  // Initialize any Electron-specific components that need the browser
  // process to be running.
  
  // TODO: Initialize Node.js here
  // TODO: Set up Electron API bindings
}

}  // namespace electron

// JNI_OnLoad is called when the library is loaded by the Java VM.
JNI_EXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved) {
  base::android::InitVM(vm);

  if (!electron::OnJNIOnLoadInit()) {
    return -1;
  }

  // Set up the native initialization hooks.
  base::android::SetNativeInitializationHook(&electron::NativeInit);
  
  // Compositor content registration is necessary for content to function.
  content::Compositor::Initialize();

  return JNI_VERSION_1_6;
}

// Called by the VM when the library is unloaded.
JNI_EXPORT void JNI_OnUnload(JavaVM* vm, void* reserved) {
  // Cleanup would go here
}

// Main function for Android executable.
// This is needed for the linker but the actual entry point is JNI_OnLoad.
int main(int argc, char* argv[]) {
  // The main function is not used on Android.
  // The real entry point is JNI_OnLoad when the library is loaded.
  return 0;
}