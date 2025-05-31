#include "shell/browser/ui/message_box.h"

#include <android/log.h>

#include "base/callback.h"
#include "base/strings/utf_string_conversions.h"
#include "shell/browser/native_window.h"

#define LOG_TAG "MessageBoxAndroid"

namespace electron {

MessageBoxSettings::MessageBoxSettings() = default;
MessageBoxSettings::MessageBoxSettings(const MessageBoxSettings&) = default;
MessageBoxSettings::~MessageBoxSettings() = default;

int ShowMessageBoxSync(const MessageBoxSettings& settings) {
  // TODO: Implement Android alert dialog
  __android_log_print(ANDROID_LOG_INFO, LOG_TAG, "ShowMessageBoxSync - Title: %s, Message: %s", 
                      base::UTF16ToUTF8(settings.title).c_str(),
                      base::UTF16ToUTF8(settings.message).c_str());
  
  // Return default button (0) for now
  return 0;
}

void ShowMessageBox(const MessageBoxSettings& settings,
                    MessageBoxCallback callback) {
  // TODO: Implement async Android alert dialog
  __android_log_print(ANDROID_LOG_INFO, LOG_TAG, "ShowMessageBox - Title: %s, Message: %s", 
                      base::UTF16ToUTF8(settings.title).c_str(),
                      base::UTF16ToUTF8(settings.message).c_str());
  
  if (callback) {
    // Return default response for now
    std::move(callback).Run(0, false);
  }
}

void ShowErrorBox(const std::u16string& title, const std::u16string& content) {
  // TODO: Implement Android error dialog
  __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "ShowErrorBox - Title: %s, Content: %s", 
                      base::UTF16ToUTF8(title).c_str(),
                      base::UTF16ToUTF8(content).c_str());
}

}  // namespace electron