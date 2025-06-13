// Copyright 2012 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "electron/android/shell_manager.h"

#include "base/android/jni_android.h"
#include "base/android/jni_string.h"
#include "base/android/scoped_java_ref.h"
#include "base/functional/bind.h"
#include "base/lazy_instance.h"
#include "content/public/browser/web_contents.h"
#include "content/shell/browser/shell.h"
#include "content/shell/browser/shell_browser_context.h"
#include "content/shell/browser/shell_content_browser_client.h"
#include "url/gurl.h"

// Must come after all headers that specialize FromJniType() / ToJniType().
#include "jni_headers/electron/android/content_shell_jni_headers/ElectronShellManager_jni.h"

using base::android::JavaParamRef;
using base::android::JavaRef;
using base::android::ScopedJavaLocalRef;

namespace {

struct GlobalState {
  GlobalState() {}
  base::android::ScopedJavaGlobalRef<jobject> j_shell_manager;
};

base::LazyInstance<GlobalState>::DestructorAtExit g_global_state =
    LAZY_INSTANCE_INITIALIZER;

}  // namespace

namespace content {

ScopedJavaLocalRef<jobject> CreateShellView(Shell* shell) {
  JNIEnv* env = base::android::AttachCurrentThread();
  return Java_ElectronShellManager_createShell(env,
                                       g_global_state.Get().j_shell_manager,
                                       reinterpret_cast<intptr_t>(shell));
}

void RemoveShellView(const JavaRef<jobject>& shell_view) {
  JNIEnv* env = base::android::AttachCurrentThread();
  Java_ElectronShellManager_removeShell(env, g_global_state.Get().j_shell_manager,
                                shell_view);
}

static void JNI_ElectronShellManager_Init(JNIEnv* env,
                                  const JavaParamRef<jobject>& obj) {
  g_global_state.Get().j_shell_manager.Reset(obj);
}

void JNI_ElectronShellManager_LaunchShell(JNIEnv* env,
                                  const JavaParamRef<jstring>& jurl) {
  ShellBrowserContext* browserContext =
      ShellContentBrowserClient::Get()->browser_context();
  GURL url(base::android::ConvertJavaStringToUTF8(env, jurl));
  Shell::CreateNewWindow(browserContext, url, nullptr, gfx::Size());
}

void DestroyShellManager() {
  JNIEnv* env = base::android::AttachCurrentThread();
  Java_ElectronShellManager_destroy(env, g_global_state.Get().j_shell_manager);
}

}  // namespace content
