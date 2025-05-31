#include "shell/common/platform_util.h"

#include <android/log.h>

#include "base/files/file_path.h"
#include "base/logging.h"
#include "url/gurl.h"

#define LOG_TAG "PlatformUtilAndroid"

namespace platform_util {

void ShowItemInFolder(const base::FilePath& full_path) {
  // TODO: Implement Android-specific file browser integration
  __android_log_print(ANDROID_LOG_INFO, LOG_TAG, "ShowItemInFolder: %s", full_path.value().c_str());
}

bool OpenItem(const base::FilePath& full_path) {
  // TODO: Implement Android-specific file opening
  __android_log_print(ANDROID_LOG_INFO, LOG_TAG, "OpenItem: %s", full_path.value().c_str());
  return false;
}

bool OpenExternal(const GURL& url,
                  const OpenExternalOptions& options,
                  OpenExternalCallback callback) {
  // TODO: Implement Android intent-based URL opening
  __android_log_print(ANDROID_LOG_INFO, LOG_TAG, "OpenExternal: %s", url.spec().c_str());
  if (callback) {
    std::move(callback).Run("Not implemented on Android");
  }
  return false;
}

bool MoveItemToTrash(const base::FilePath& full_path, bool delete_on_fail) {
  // TODO: Implement Android-specific file deletion
  __android_log_print(ANDROID_LOG_INFO, LOG_TAG, "MoveItemToTrash: %s", full_path.value().c_str());
  return false;
}

void Beep() {
  // TODO: Implement Android-specific beep/vibration
  __android_log_print(ANDROID_LOG_INFO, LOG_TAG, "Beep called");
}

bool GetDesktopName(std::string* setme) {
  // Android doesn't have a traditional desktop environment
  *setme = "Android";
  return true;
}

}  // namespace platform_util