// Copyright 2024 The Electron Authors. All rights reserved.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#ifndef ELECTRON_SHELL_BROWSER_NATIVE_WINDOW_ANDROID_H_
#define ELECTRON_SHELL_BROWSER_NATIVE_WINDOW_ANDROID_H_

#include <memory>

#include "base/android/jni_weak_ref.h"
#include "base/android/scoped_java_ref.h"
#include "gin/handle.h"
#include "shell/browser/native_window.h"
#include "shell/common/gin_helper/dictionary.h"

namespace electron {

class NativeWindowAndroid : public NativeWindow {
 public:
  NativeWindowAndroid(const gin_helper::Dictionary& options,
                      NativeWindow* parent);
  ~NativeWindowAndroid() override;

  // NativeWindow implementation
  void SetContentView(views::View* view) override {}
  void CloseImpl() override;
  void CloseImmediatelyImpl() override;
  void Focus(bool focus) override;
  bool IsFocused() const override;
  void Show() override;
  void ShowInactive() override;
  void Hide() override;
  bool IsVisible() const override;
  bool IsEnabled() const override;
  void SetEnabled(bool enable) override;
  void Maximize() override;
  void Unmaximize() override;
  bool IsMaximized() const override;
  void Minimize() override;
  void Restore() override;
  bool IsMinimized() const override;
  void SetFullScreen(bool fullscreen) override;
  bool IsFullscreen() const override;
  void SetBounds(const gfx::Rect& bounds, bool animate = false) override;
  gfx::Rect GetBounds() const override;
  gfx::Rect GetContentBounds() const override;
  gfx::Size GetContentSize() const override;
  gfx::Rect GetNormalBounds() const override;
  void SetContentSizeConstraints(
      const extensions::SizeConstraints& size_constraints) override;
  void SetResizable(bool resizable) override;
  bool MoveAbove(const std::string& sourceId) override;
  void MoveTop() override;
  bool IsResizable() const override;
  void SetMovable(bool movable) override;
  bool IsMovable() const override;
  void SetMinimizable(bool minimizable) override;
  bool IsMinimizable() const override;
  void SetMaximizable(bool maximizable) override;
  bool IsMaximizable() const override;
  void SetFullScreenable(bool fullscreenable) override;
  bool IsFullScreenable() const override;
  void SetClosable(bool closable) override;
  bool IsClosable() const override;
  void SetAlwaysOnTop(ui::ZOrderLevel z_order,
                      const std::string& level = "floating",
                      int relativeLevel = 0) override;
  ui::ZOrderLevel GetZOrderLevel() const override;
  void Center() override;
  void Invalidate() override;
  bool IsActive() const override;
  // SetTitle and GetTitle are non-virtual in base class
  void FlashFrame(bool flash) override;
  void SetSkipTaskbar(bool skip) override;
  void SetExcludedFromShownWindowsMenu(bool excluded) override;
  bool IsExcludedFromShownWindowsMenu() const override;
  void SetSimpleFullScreen(bool simple_fullscreen) override;
  bool IsSimpleFullScreen() const override;
  void SetKiosk(bool kiosk) override;
  bool IsKiosk() const override;
  void SetBackgroundColor(SkColor color) override;
  SkColor GetBackgroundColor() const override;
  void SetHasShadow(bool has_shadow) override;
  bool HasShadow() const override;
  void SetOpacity(const double opacity) override;
  double GetOpacity() const override;
  void SetIgnoreMouseEvents(bool ignore, bool forward) override;
  void SetContentProtection(bool enable) override;
  bool IsContentProtected() const override;
  void SetFocusable(bool focusable) override;
  bool IsFocusable() const override;
  void SetMenu(ElectronMenuModel* menu) override;
  void SetParentWindow(NativeWindow* parent) override;
  content::DesktopMediaID GetDesktopMediaID() const override;
  gfx::NativeView GetNativeView() const override;
  gfx::NativeWindow GetNativeWindow() const override;
  gfx::AcceleratedWidget GetAcceleratedWidget() const override;
  NativeWindowHandle GetNativeWindowHandle() const override;
  void SetProgressBar(double progress, const ProgressState state) override;
  void SetOverlayIcon(const gfx::Image& overlay,
                      const std::string& description) override;
  void SetVisibleOnAllWorkspaces(bool visible,
                                  bool visibleOnFullScreen = false,
                                  bool skipTransformProcessType = false) override;
  bool IsVisibleOnAllWorkspaces() const override;
  void SetAutoHideCursor(bool auto_hide) override;
  void SetVibrancy(const std::string& type, int duration) override;
  void SetBackgroundMaterial(const std::string& type) override;

  // Android-specific methods
  void OnActivityCreated(JNIEnv* env,
                         const base::android::JavaParamRef<jobject>& obj);
  void OnActivityDestroyed(JNIEnv* env,
                            const base::android::JavaParamRef<jobject>& obj);
  void OnActivityStarted(JNIEnv* env,
                         const base::android::JavaParamRef<jobject>& obj);
  void OnActivityStopped(JNIEnv* env,
                         const base::android::JavaParamRef<jobject>& obj);
  void OnWindowFocusChanged(JNIEnv* env,
                            const base::android::JavaParamRef<jobject>& obj,
                            bool has_focus);

  // Get the Java window object
  base::android::ScopedJavaLocalRef<jobject> GetJavaObject();

  // NativeWindow implementation - bounds conversion
  gfx::Rect ContentBoundsToWindowBounds(const gfx::Rect& bounds) const override;
  gfx::Rect WindowBoundsToContentBounds(const gfx::Rect& bounds) const override;

 private:
  // Create the Android window
  void CreateAndroidWindow();

  // Java object that represents the window
  base::android::ScopedJavaGlobalRef<jobject> java_window_;

  // Window bounds
  gfx::Rect bounds_;

  // Window state
  bool is_fullscreen_ = false;
  bool is_visible_ = true;
  bool is_focused_ = false;
  bool is_enabled_ = true;
  bool is_always_on_top_ = false;
  bool is_kiosk_ = false;
  bool is_resizable_ = true;
  bool is_movable_ = true;
  bool is_minimizable_ = true;
  bool is_maximizable_ = true;
  bool is_closable_ = true;
  bool is_focusable_ = true;
  bool has_shadow_ = true;
  
  // Window properties
  std::string title_;
  SkColor background_color_ = SK_ColorWHITE;
  double opacity_ = 1.0;

  NativeWindowAndroid(const NativeWindowAndroid&) = delete;
  NativeWindowAndroid& operator=(const NativeWindowAndroid&) = delete;
};

}  // namespace electron

#endif  // ELECTRON_SHELL_BROWSER_NATIVE_WINDOW_ANDROID_H_