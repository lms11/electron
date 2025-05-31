#include "shell/common/language_util.h"

#include <locale.h>
#include <android/log.h>

#include "base/strings/string_split.h"

#define LOG_TAG "LanguageUtilAndroid"

namespace electron {

std::vector<std::string> GetPreferredLanguages() {
  std::vector<std::string> languages;
  
  // Get system locale
  const char* locale_str = setlocale(LC_ALL, nullptr);
  if (locale_str && strlen(locale_str) > 0) {
    std::string locale(locale_str);
    
    // Convert locale format (e.g., "en_US.UTF-8") to language format (e.g., "en-US")
    if (locale.find('.') != std::string::npos) {
      locale = locale.substr(0, locale.find('.'));
    }
    if (locale.find('_') != std::string::npos) {
      locale = locale.replace(locale.find('_'), 1, "-");
    }
    
    languages.push_back(locale);
    __android_log_print(ANDROID_LOG_INFO, LOG_TAG, "System locale: %s", locale.c_str());
  }
  
  // Default fallback
  if (languages.empty()) {
    languages.push_back("en-US");
    __android_log_print(ANDROID_LOG_INFO, LOG_TAG, "Using default locale: en-US");
  }
  
  return languages;
}

}  // namespace electron