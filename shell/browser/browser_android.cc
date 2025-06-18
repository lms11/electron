// Copyright (c) 2024 The Electron Authors.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#include "shell/browser/browser.h"

#include "shell/browser/javascript_environment.h"
#include "shell/common/application_info.h"
#include "shell/common/gin_converters/login_item_settings_converter.h"

namespace electron {

void Browser::AddRecentDocument(const base::FilePath& path) {
  // Not implemented on Android
}

void Browser::ClearRecentDocuments() {
  // Not implemented on Android
}

bool Browser::SetAsDefaultProtocolClient(const std::string& protocol,
                                         gin::Arguments* args) {
  // Not implemented on Android
  return false;
}

bool Browser::IsDefaultProtocolClient(const std::string& protocol,
                                      gin::Arguments* args) {
  // Not implemented on Android
  return false;
}

bool Browser::RemoveAsDefaultProtocolClient(const std::string& protocol,
                                            gin::Arguments* args) {
  // Not implemented on Android
  return false;
}

std::u16string Browser::GetApplicationNameForProtocol(const GURL& url) {
  // Not implemented on Android
  return std::u16string();
}

bool Browser::SetBadgeCount(std::optional<int> count) {
  // Not implemented on Android
  return false;
}

void Browser::SetLoginItemSettings(LoginItemSettings settings) {
  // Not implemented on Android
}

v8::Local<v8::Value> Browser::GetLoginItemSettings(
    const LoginItemSettings& options) {
  // Return empty settings for Android
  LoginItemSettings settings;
  return gin::ConvertToV8(JavascriptEnvironment::GetIsolate(), settings);
}

std::string Browser::GetExecutableFileVersion() const {
  return GetApplicationVersion();
}

std::string Browser::GetExecutableFileProductName() const {
  return GetApplicationName();
}

bool Browser::IsEmojiPanelSupported() {
  // Not supported on Android
  return false;
}

void Browser::ShowAboutPanel() {
  // Not implemented on Android
}

void Browser::SetAboutPanelOptions(base::Value::Dict options) {
  about_panel_options_ = std::move(options);
}

}  // namespace electron