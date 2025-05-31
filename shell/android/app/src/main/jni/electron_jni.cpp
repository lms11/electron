#include <jni.h>
#include <android/log.h>
#include <string>

#include "base/at_exit.h"
#include "base/command_line.h"
#include "base/i18n/icu_util.h"
#include "shell/app/node_main.h"
#include "shell/common/node_bindings.h"

#define LOG_TAG "ElectronJNI"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

namespace {
  bool g_initialized = false;
  base::AtExitManager* g_at_exit_manager = nullptr;
}

extern "C" JNIEXPORT void JNICALL
Java_org_electronjs_app_ElectronActivity_runNodeHelloWorld(JNIEnv *env, jobject thiz) {
    LOGI("Starting Node.js Hello World initialization");
    
    if (!g_initialized) {
        // Initialize base systems
        g_at_exit_manager = new base::AtExitManager();
        
        // Initialize command line (required for Node.js)
        const char* argv[] = {"electron", "--enable-logging", "--log-level=0"};
        base::CommandLine::Init(3, argv);
        
        // Initialize ICU (required for V8/Node.js)
        if (!base::i18n::InitializeICU()) {
            LOGE("Failed to initialize ICU");
            return;
        }
        
        g_initialized = true;
        LOGI("Base systems initialized successfully");
    }
    
    try {
        // This is where we'll eventually call electron::NodeMain()
        // For now, just log success
        LOGI("Hello from Node.js on Android!");
        LOGI("Electron Node.js integration ready");
        
        // TODO: Initialize Node.js runtime properly
        // electron::NodeMain();
        
    } catch (const std::exception& e) {
        LOGE("Exception in Node.js initialization: %s", e.what());
    } catch (...) {
        LOGE("Unknown exception in Node.js initialization");
    }
}