#include "shell/browser/ui/file_dialog.h"

#include <android/log.h>

#include "base/callback.h"
#include "base/files/file_path.h"

#define LOG_TAG "FileDialogAndroid"

namespace file_dialog {

void ShowOpenDialogSync(const DialogSettings& settings,
                        std::vector<base::FilePath>* paths) {
  // TODO: Implement Android file picker
  __android_log_print(ANDROID_LOG_INFO, LOG_TAG, "ShowOpenDialogSync called");
  if (paths) {
    paths->clear();
  }
}

void ShowOpenDialog(const DialogSettings& settings,
                    std::vector<base::FilePath>* paths,
                    gin::Arguments* args) {
  // TODO: Implement async Android file picker
  __android_log_print(ANDROID_LOG_INFO, LOG_TAG, "ShowOpenDialog called");
  if (paths) {
    paths->clear();
  }
}

void ShowSaveDialogSync(const DialogSettings& settings,
                        base::FilePath* path) {
  // TODO: Implement Android save dialog
  __android_log_print(ANDROID_LOG_INFO, LOG_TAG, "ShowSaveDialogSync called");
  if (path) {
    *path = base::FilePath();
  }
}

void ShowSaveDialog(const DialogSettings& settings,
                    base::FilePath* path,
                    gin::Arguments* args) {
  // TODO: Implement async Android save dialog
  __android_log_print(ANDROID_LOG_INFO, LOG_TAG, "ShowSaveDialog called");
  if (path) {
    *path = base::FilePath();
  }
}

}  // namespace file_dialog