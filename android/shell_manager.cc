// Copyright 2012 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "electron/android/shell_manager.h"

#include "base/android/jni_android.h"
#include "base/android/jni_string.h"
#include "base/android/scoped_java_ref.h"
#include "base/functional/bind.h"
#include "base/lazy_instance.h"
#include "base/logging.h"
#include "content/public/browser/browser_context.h"
#include "content/public/browser/navigation_controller.h"
#include "content/public/browser/navigation_entry.h"
#include "content/public/browser/navigation_handle.h"
#include "content/public/browser/render_process_host.h"
#include "content/public/browser/render_view_host.h"
#include "content/public/browser/render_widget_host.h"
#include "content/public/browser/render_widget_host_view.h"
#include "content/public/browser/web_contents.h"
#include "content/public/browser/web_contents_delegate.h"
#include "content/public/browser/web_contents_observer.h"
#include "content/public/browser/child_process_termination_info.h"
#include "content/shell/browser/shell_browser_context.h"
#include "ui/android/view_android.h"
#include "content/shell/browser/shell_content_browser_client.h"
#include "electron/shell/browser/electron_browser_client.h"
#include "electron/shell/browser/electron_browser_context.h"
#include "ui/base/page_transition_types.h"
#include "url/gurl.h"

// Must come after all headers that specialize FromJniType() / ToJniType().
#include "jni_headers/electron/android/content_shell_jni_headers/ElectronShellManager_jni.h"
#include "jni_headers/electron/android/content_shell_jni_headers/ElectronShell_jni.h"

using base::android::JavaParamRef;
using base::android::JavaRef;
using base::android::ScopedJavaLocalRef;

namespace {

class SimpleWebContentsDelegate;

struct GlobalState {
  GlobalState() {}
  base::android::ScopedJavaGlobalRef<jobject> j_shell_manager;
  // Map from raw WebContents to Java ElectronShell object
  std::map<content::WebContents*, base::android::ScopedJavaGlobalRef<jobject>> web_contents_to_java_map;
  // Map from raw WebContents to delegate/observer
  std::map<content::WebContents*, std::unique_ptr<SimpleWebContentsDelegate>> delegate_map;
  // Map to keep WebContents alive
  std::map<content::WebContents*, std::unique_ptr<content::WebContents>> owned_web_contents;
};

base::LazyInstance<GlobalState>::DestructorAtExit g_global_state =
    LAZY_INSTANCE_INITIALIZER;

// Simple WebContentsDelegate for Android
class SimpleWebContentsDelegate : public content::WebContentsDelegate,
                                  public content::WebContentsObserver {
 public:
  explicit SimpleWebContentsDelegate(content::WebContents* web_contents) 
      : content::WebContentsObserver(web_contents),
        weak_factory_(this) {}
  ~SimpleWebContentsDelegate() override = default;
  
  // WebContentsDelegate overrides
  content::WebContents* OpenURLFromTab(
      content::WebContents* source,
      const content::OpenURLParams& params,
      base::OnceCallback<void(content::NavigationHandle&)>
          navigation_handle_callback) override {
    if (params.disposition != WindowOpenDisposition::CURRENT_TAB) {
      return nullptr;
    }
    
    source->GetController().LoadURLWithParams(
        content::NavigationController::LoadURLParams(params));
    return source;
  }
  
  // Override critical delegate methods that might block navigation
  bool ShouldAllowRunningInsecureContent(content::WebContents* web_contents,
                                        bool allowed_per_prefs,
                                        const url::Origin& origin,
                                        const GURL& resource_url) override {
    return true;  // Allow all for now
  }
  
  bool ShouldResumeRequestsForCreatedWindow() override {
    return true;
  }
  
  // Add more delegate methods that might be blocking navigation  
  void CloseContents(content::WebContents* source) override {
    // Don't close - we want to keep the contents
  }
  
 private:
  base::WeakPtrFactory<SimpleWebContentsDelegate> weak_factory_;
};

}  // namespace

namespace content {

static void JNI_ElectronShellManager_Init(JNIEnv* env,
                                  const JavaParamRef<jobject>& obj) {
  g_global_state.Get().j_shell_manager.Reset(obj);
}

void JNI_ElectronShellManager_LaunchShell(JNIEnv* env,
                                  const JavaParamRef<jstring>& jurl) {
  // Use Electron's browser context
  BrowserContext* browser_context = electron::ElectronBrowserContext::From("", false);
  
  if (!browser_context) {
    return;
  }
  
  // Create WebContents - simple approach like content shell
  WebContents::CreateParams create_params(browser_context, nullptr);
  std::unique_ptr<WebContents> web_contents = WebContents::Create(create_params);
  WebContents* raw_web_contents = web_contents.get();
  
  
  // Set the delegate - this might be needed for navigation to work
  auto delegate = std::make_unique<SimpleWebContentsDelegate>(raw_web_contents);
  SimpleWebContentsDelegate* delegate_ptr = delegate.get();
  raw_web_contents->SetDelegate(delegate_ptr);
  g_global_state.Get().delegate_map[raw_web_contents] = std::move(delegate);
  
  // Create the Java shell view
  base::android::ScopedJavaLocalRef<jobject> java_shell = 
      Java_ElectronShellManager_createShell(env,
                                           g_global_state.Get().j_shell_manager,
                                           reinterpret_cast<intptr_t>(raw_web_contents));
  
  // Store the mapping
  g_global_state.Get().web_contents_to_java_map[raw_web_contents] = 
      base::android::ScopedJavaGlobalRef<jobject>(java_shell);
  
  // Initialize the Java shell with WebContents
  Java_ElectronShell_initFromNativeTabContents(
      env, java_shell, raw_web_contents->GetJavaWebContents());
  
  // Store the WebContents to keep it alive BEFORE loading URL
  g_global_state.Get().owned_web_contents[raw_web_contents] = std::move(web_contents);
  
  // Load the URL
  GURL url(base::android::ConvertJavaStringToUTF8(env, jurl));
  
  if (!url.is_empty()) {
    // Force visibility update first
    raw_web_contents->UpdateWebContentsVisibility(content::Visibility::VISIBLE);
    
    // Focus the WebContents
    raw_web_contents->Focus();
    
    // Load URL immediately like Content Shell does
    content::NavigationController::LoadURLParams params(url);
    params.transition_type = ui::PageTransitionFromInt(
        ui::PAGE_TRANSITION_TYPED | ui::PAGE_TRANSITION_FROM_ADDRESS_BAR);
    raw_web_contents->GetController().LoadURLWithParams(params);
  }
}

static void JNI_ElectronShell_CloseShell(JNIEnv* env, jlong webContentsPtr) {
  content::WebContents* web_contents = reinterpret_cast<content::WebContents*>(webContentsPtr);
  
  // Remove from Java map
  auto it = g_global_state.Get().web_contents_to_java_map.find(web_contents);
  if (it != g_global_state.Get().web_contents_to_java_map.end()) {
    // Call onNativeDestroyed on the Java object
    if (!it->second.is_null()) {
      Java_ElectronShell_onNativeDestroyed(env, it->second);
    }
    
    // Remove the Java shell view
    Java_ElectronShellManager_removeShell(env, 
                                         g_global_state.Get().j_shell_manager,
                                         it->second);
    g_global_state.Get().web_contents_to_java_map.erase(it);
  }
  
  // Clean up observer delegate
  g_global_state.Get().delegate_map.erase(web_contents);
  
  // Release WebContents ownership - this will delete it
  g_global_state.Get().owned_web_contents.erase(web_contents);
}

void DestroyElectronShellManager() {
  JNIEnv* env = base::android::AttachCurrentThread();
  
  // Clean up all remaining WebContents
  g_global_state.Get().web_contents_to_java_map.clear();
  g_global_state.Get().delegate_map.clear();
  g_global_state.Get().owned_web_contents.clear();  // This will delete all WebContents
  
  Java_ElectronShellManager_destroy(env, g_global_state.Get().j_shell_manager);
}

}  // namespace content
