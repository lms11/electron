// Copyright 2024 The Electron Authors. All rights reserved.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#include "shell/browser/native_window_android.h"

#include "base/android/jni_android.h"
#include "base/android/jni_string.h"
#include "base/android/scoped_java_ref.h"
#include "base/strings/utf_string_conversions.h"
#include "content/public/browser/web_contents.h"
#include "shell/browser/browser.h"
#include "shell/browser/ui/inspectable_web_contents.h"
#include "shell/browser/ui/inspectable_web_contents_view.h"
#include "shell/common/options_switches.h"
#include "shell/common/gin_converters/gfx_converter.h"
#include "ui/gfx/geometry/rect.h"

// TODO: Include generated JNI headers
// #include "electron/shell/android/electron_jni_headers/ElectronWindow_jni.h"

namespace electron {

NativeWindowAndroid::NativeWindowAndroid(const gin_helper::Dictionary& options,
                                         NativeWindow* parent)
    : NativeWindow(options, parent) {
  // Set default window properties
  options.Get(options::kTitle, &title_);
  options.Get(options::kShow, &is_visible_);
  options.Get(options::kFullscreen, &is_fullscreen_);
  options.Get(options::kResizable, &is_resizable_);
  options.Get(options::kClosable, &is_closable_);
  options.Get(options::kFocusable, &is_focusable_);
  options.Get(options::kAlwaysOnTop, &is_always_on_top_);
  options.Get(options::kKiosk, &is_kiosk_);

  // Get window bounds
  int x = 0, y = 0, width = 800, height = 600;
  if (options.Get(options::kX, &x) && options.Get(options::kY, &y)) {
    // Position specified
  }
  options.Get(options::kWidth, &width);
  options.Get(options::kHeight, &height);
  bounds_.SetRect(x, y, width, height);

  // Get background color
  std::string color;
  if (options.Get(options::kBackgroundColor, &color)) {
    // TODO: Parse CSS color string to SkColor
    // background_color_ = ParseCSSColor(color);
  }

  // Create the Android window
  CreateAndroidWindow();
}

NativeWindowAndroid::~NativeWindowAndroid() {
  // Cleanup will be handled by Java side
}

void NativeWindowAndroid::CreateAndroidWindow() {
  // JNIEnv* env = base::android::AttachCurrentThread();
  
  // TODO: Create Java ElectronWindow object
  // java_window_.Reset(Java_ElectronWindow_create(env, reinterpret_cast<intptr_t>(this)));
  
  // TODO: Set initial properties on the Java window
  // if (!java_window_.is_null()) {
  //   Java_ElectronWindow_setTitle(env, java_window_, 
  //       base::android::ConvertUTF8ToJavaString(env, title_));
  //   Java_ElectronWindow_setBounds(env, java_window_, 
  //       bounds_.x(), bounds_.y(), bounds_.width(), bounds_.height());
  //   Java_ElectronWindow_setFullscreen(env, java_window_, is_fullscreen_);
  //   Java_ElectronWindow_setVisible(env, java_window_, is_visible_);
  // }
}

void NativeWindowAndroid::CloseImpl() {
  if (!java_window_.is_null()) {
    // JNIEnv* env = base::android::AttachCurrentThread();
    // TODO: Call Java close method
    // Java_ElectronWindow_close(env, java_window_);
  }
  
  NotifyWindowClosed();
}

void NativeWindowAndroid::CloseImmediatelyImpl() {
  NotifyWindowClosed();
}

void NativeWindowAndroid::Focus(bool focus) {
  if (!java_window_.is_null() && focus) {
    // JNIEnv* env = base::android::AttachCurrentThread();
    // TODO: Call Java focus method
    // Java_ElectronWindow_focus(env, java_window_);
  }
}

bool NativeWindowAndroid::IsFocused() const {
  return is_focused_;
}

void NativeWindowAndroid::Show() {
  is_visible_ = true;
  if (!java_window_.is_null()) {
    // JNIEnv* env = base::android::AttachCurrentThread();
    // TODO: Call Java show method
    // Java_ElectronWindow_setVisible(env, java_window_, true);
  }
}

void NativeWindowAndroid::ShowInactive() {
  // Android doesn't support showing without focusing
  Show();
}

void NativeWindowAndroid::Hide() {
  is_visible_ = false;
  if (!java_window_.is_null()) {
    // JNIEnv* env = base::android::AttachCurrentThread();
    // TODO: Call Java hide method
    // Java_ElectronWindow_setVisible(env, java_window_, false);
  }
}

bool NativeWindowAndroid::IsVisible() const {
  return is_visible_;
}

bool NativeWindowAndroid::IsEnabled() const {
  return is_enabled_;
}

void NativeWindowAndroid::SetEnabled(bool enable) {
  is_enabled_ = enable;
}

void NativeWindowAndroid::Maximize() {
  // Android doesn't have traditional window maximize
  // Could implement as fullscreen
  SetFullScreen(true);
}

void NativeWindowAndroid::Unmaximize() {
  SetFullScreen(false);
}

bool NativeWindowAndroid::IsMaximized() const {
  return is_fullscreen_;
}

void NativeWindowAndroid::Minimize() {
  // On Android, minimize means going to background
  if (!java_window_.is_null()) {
    // JNIEnv* env = base::android::AttachCurrentThread();
    // TODO: Move activity to background
  }
}

void NativeWindowAndroid::Restore() {
  if (is_fullscreen_) {
    SetFullScreen(false);
  }
  Show();
}

bool NativeWindowAndroid::IsMinimized() const {
  // On Android, minimized means in background
  return !is_visible_;
}

void NativeWindowAndroid::SetFullScreen(bool fullscreen) {
  is_fullscreen_ = fullscreen;
  if (!java_window_.is_null()) {
    // JNIEnv* env = base::android::AttachCurrentThread();
    // TODO: Call Java fullscreen method
    // Java_ElectronWindow_setFullscreen(env, java_window_, fullscreen);
  }
}

bool NativeWindowAndroid::IsFullscreen() const {
  return is_fullscreen_;
}

void NativeWindowAndroid::SetBounds(const gfx::Rect& bounds, bool animate) {
  bounds_ = bounds;
  if (!java_window_.is_null()) {
    // JNIEnv* env = base::android::AttachCurrentThread();
    // TODO: Call Java setBounds method
    // Java_ElectronWindow_setBounds(env, java_window_,
    //     bounds.x(), bounds.y(), bounds.width(), bounds.height());
  }
}

gfx::Rect NativeWindowAndroid::GetBounds() const {
  return bounds_;
}

gfx::Rect NativeWindowAndroid::GetContentBounds() const {
  return bounds_;
}

gfx::Size NativeWindowAndroid::GetContentSize() const {
  return bounds_.size();
}

gfx::Rect NativeWindowAndroid::GetNormalBounds() const {
  // On Android, normal bounds are the same as current bounds
  return bounds_;
}

void NativeWindowAndroid::SetContentSizeConstraints(
    const extensions::SizeConstraints& size_constraints) {
  // TODO: Implement size constraints on Android
}

void NativeWindowAndroid::SetResizable(bool resizable) {
  is_resizable_ = resizable;
}

bool NativeWindowAndroid::MoveAbove(const std::string& sourceId) {
  // Not supported on Android
  return false;
}

void NativeWindowAndroid::MoveTop() {
  // Bring activity to front on Android
  Focus(true);
}

bool NativeWindowAndroid::IsResizable() const {
  return is_resizable_;
}

void NativeWindowAndroid::SetMovable(bool movable) {
  is_movable_ = movable;
}

bool NativeWindowAndroid::IsMovable() const {
  return is_movable_;
}

void NativeWindowAndroid::SetMinimizable(bool minimizable) {
  is_minimizable_ = minimizable;
}

bool NativeWindowAndroid::IsMinimizable() const {
  return is_minimizable_;
}

void NativeWindowAndroid::SetMaximizable(bool maximizable) {
  is_maximizable_ = maximizable;
}

bool NativeWindowAndroid::IsMaximizable() const {
  return is_maximizable_;
}

void NativeWindowAndroid::SetFullScreenable(bool fullscreenable) {
  // Android windows are always fullscreenable
}

bool NativeWindowAndroid::IsFullScreenable() const {
  return true;
}

void NativeWindowAndroid::SetClosable(bool closable) {
  is_closable_ = closable;
}

bool NativeWindowAndroid::IsClosable() const {
  return is_closable_;
}

void NativeWindowAndroid::SetAlwaysOnTop(ui::ZOrderLevel z_order,
                                          const std::string& level,
                                          int relativeLevel) {
  is_always_on_top_ = (z_order != ui::ZOrderLevel::kNormal);
}

ui::ZOrderLevel NativeWindowAndroid::GetZOrderLevel() const {
  return is_always_on_top_ ? ui::ZOrderLevel::kFloatingWindow 
                           : ui::ZOrderLevel::kNormal;
}

void NativeWindowAndroid::Center() {
  // Centering doesn't make sense on Android
}

void NativeWindowAndroid::Invalidate() {
  // TODO: Invalidate the view
}

bool NativeWindowAndroid::IsActive() const {
  return is_focused_;
}

void NativeWindowAndroid::FlashFrame(bool flash) {
  // Not supported on Android
}

void NativeWindowAndroid::SetSkipTaskbar(bool skip) {
  // Not applicable on Android
}

void NativeWindowAndroid::SetExcludedFromShownWindowsMenu(bool excluded) {
  // Not applicable on Android
}

bool NativeWindowAndroid::IsExcludedFromShownWindowsMenu() const {
  return false;
}

void NativeWindowAndroid::SetSimpleFullScreen(bool simple_fullscreen) {
  SetFullScreen(simple_fullscreen);
}

bool NativeWindowAndroid::IsSimpleFullScreen() const {
  return IsFullscreen();
}

void NativeWindowAndroid::SetKiosk(bool kiosk) {
  is_kiosk_ = kiosk;
  if (kiosk) {
    SetFullScreen(true);
  }
}

bool NativeWindowAndroid::IsKiosk() const {
  return is_kiosk_;
}

void NativeWindowAndroid::SetBackgroundColor(SkColor color) {
  background_color_ = color;
  // TODO: Apply to Android window
}

SkColor NativeWindowAndroid::GetBackgroundColor() const {
  return background_color_;
}

void NativeWindowAndroid::SetHasShadow(bool has_shadow) {
  has_shadow_ = has_shadow;
}

bool NativeWindowAndroid::HasShadow() const {
  return has_shadow_;
}

void NativeWindowAndroid::SetOpacity(const double opacity) {
  opacity_ = opacity;
  // TODO: Apply to Android window
}

double NativeWindowAndroid::GetOpacity() const {
  return opacity_;
}

void NativeWindowAndroid::SetIgnoreMouseEvents(bool ignore, bool forward) {
  // Touch events on Android, not mouse
}

void NativeWindowAndroid::SetContentProtection(bool enable) {
  // TODO: Implement using FLAG_SECURE
}

bool NativeWindowAndroid::IsContentProtected() const {
  // TODO: Check FLAG_SECURE status
  return false;
}

void NativeWindowAndroid::SetFocusable(bool focusable) {
  is_focusable_ = focusable;
}

bool NativeWindowAndroid::IsFocusable() const {
  return is_focusable_;
}

void NativeWindowAndroid::SetMenu(ElectronMenuModel* menu) {
  // TODO: Implement Android menu
}

void NativeWindowAndroid::SetParentWindow(NativeWindow* parent) {
  // Not supported on Android
}

content::DesktopMediaID NativeWindowAndroid::GetDesktopMediaID() const {
  // Return empty ID for Android
  return content::DesktopMediaID();
}

gfx::NativeView NativeWindowAndroid::GetNativeView() const {
  // TODO: Return the Android view
  return nullptr;
}

gfx::NativeWindow NativeWindowAndroid::GetNativeWindow() const {
  // TODO: Return the Android window
  return nullptr;
}

gfx::AcceleratedWidget NativeWindowAndroid::GetAcceleratedWidget() const {
  // TODO: Return the surface for rendering
  return gfx::kNullAcceleratedWidget;
}

NativeWindowHandle NativeWindowAndroid::GetNativeWindowHandle() const {
  return nullptr;
}

void NativeWindowAndroid::SetProgressBar(double progress,
                                         const ProgressState state) {
  // Could implement with notification
}

void NativeWindowAndroid::SetOverlayIcon(const gfx::Image& overlay,
                                         const std::string& description) {
  // Not supported on Android
}

void NativeWindowAndroid::SetVisibleOnAllWorkspaces(
    bool visible,
    bool visibleOnFullScreen,
    bool skipTransformProcessType) {
  // Not applicable on Android
}

bool NativeWindowAndroid::IsVisibleOnAllWorkspaces() const {
  return false;
}

void NativeWindowAndroid::SetAutoHideCursor(bool auto_hide) {
  // Not applicable on Android (touch interface)
}

void NativeWindowAndroid::SetVibrancy(const std::string& type, int duration) {
  // Not supported on Android
}

void NativeWindowAndroid::SetBackgroundMaterial(const std::string& type) {
  // Not supported on Android
}

// Methods removed as they're not in the base class for Android

// Android-specific JNI methods
void NativeWindowAndroid::OnActivityCreated(
    JNIEnv* env,
    const base::android::JavaParamRef<jobject>& obj) {
  // Activity was created
}

void NativeWindowAndroid::OnActivityDestroyed(
    JNIEnv* env,
    const base::android::JavaParamRef<jobject>& obj) {
  // Activity was destroyed
  CloseImmediately();
}

void NativeWindowAndroid::OnActivityStarted(
    JNIEnv* env,
    const base::android::JavaParamRef<jobject>& obj) {
  // Activity started
}

void NativeWindowAndroid::OnActivityStopped(
    JNIEnv* env,
    const base::android::JavaParamRef<jobject>& obj) {
  // Activity stopped
}

void NativeWindowAndroid::OnWindowFocusChanged(
    JNIEnv* env,
    const base::android::JavaParamRef<jobject>& obj,
    bool has_focus) {
  is_focused_ = has_focus;
  if (has_focus) {
    NotifyWindowFocus();
  } else {
    NotifyWindowBlur();
  }
}

base::android::ScopedJavaLocalRef<jobject> NativeWindowAndroid::GetJavaObject() {
  return base::android::ScopedJavaLocalRef<jobject>(java_window_);
}

gfx::Rect NativeWindowAndroid::ContentBoundsToWindowBounds(
    const gfx::Rect& bounds) const {
  // On Android, content bounds and window bounds are the same
  return bounds;
}

gfx::Rect NativeWindowAndroid::WindowBoundsToContentBounds(
    const gfx::Rect& bounds) const {
  // On Android, content bounds and window bounds are the same
  return bounds;
}

// static
std::unique_ptr<NativeWindow> NativeWindow::Create(const gin_helper::Dictionary& options,
                                                   NativeWindow* parent) {
  return std::make_unique<NativeWindowAndroid>(options, parent);
}

}  // namespace electron