# Electron Android Support Implementation Plan

## Overview

This document outlines a comprehensive plan for adding Android support to Electron. The approach uses incremental milestones starting with basic "Hello World" implementations and building toward full Android support.

**Key Architectural Decision:** This plan preserves Electron's security model by using Android Services with separate processes, rather than a threading approach that would compromise security boundaries.

**Node.js Strategy:** Uses classical Node.js with Electron's existing patch system, integrating Android-specific changes from the proven [nodejs/node android-build branch](https://github.com/nodejs/node/compare/main...shivramk:node:android-build).

## Background Analysis

### Current State
- **Existing Android stub**: `shell/app/electron_main_android.cc` exists but is minimal
- **Platform architecture**: Clear separation with `lib_sources_linux`, `lib_sources_mac`, `lib_sources_win` patterns
- **Build system**: Uses GN/Ninja which supports Android cross-compilation
- **Dependencies**: Both Chromium and Node.js have established Android support

### Chromium Android Integration
- Uses `target_os = "android"` with ARM/ARM64 architectures
- JNI bridge system with automated binding generation (~57 files, ~9000 lines)
- ChromeTabbedActivity pattern for native-Java integration
- Shared library approach with explicit JNI registration

### Node.js Android Integration
- Use classical Node.js with Electron's existing patch system (51 patches)
- Integrate Android Node.js changes from [nodejs/node android-build branch](https://github.com/nodejs/node/compare/main...shivramk:node:android-build)
- Leverage Electron's GN build system for Node.js (replaces GYP)
- Use existing `//third_party/electron_node:libnode` build target

## Design Philosophy and Architectural Decisions

### Incremental Milestone Approach
This plan was designed with concrete, testable milestones to provide:
- **Clear progress markers**: Each milestone has specific deliverables
- **Risk mitigation**: Issues can be identified and addressed early
- **Stakeholder visibility**: Progress can be demonstrated at each stage
- **Technical validation**: Each step proves the approach works before proceeding

### Critical Architectural Decisions Made

#### 1. Process-Based vs Thread-Based IPC
**Initial Consideration:** Running Node.js and Chromium in the same process with different threads
**Final Decision:** Use Android Services with separate processes
**Reasoning:** Threading approach would have **compromised Electron's fundamental security model**:
- Process boundaries provide memory isolation
- Chromium's sandbox relies on process-level restrictions  
- V8 isolates assume process boundaries for security
- Crash isolation prevents one component from bringing down the entire app

#### 2. Classical Node.js vs Mobile Variants
**Initial Consideration:** Using Node.js for Mobile Apps or modified node-gyp
**Final Decision:** Use classical Node.js with Electron's existing patch system
**Reasoning:** 
- Leverages proven Android Node.js changes from nodejs/node android-build branch
- Maintains consistency with Electron's architecture (51 existing patches)
- Easier to maintain and update Node.js versions
- All Electron's Node.js customizations work on Android

#### 3. Cross-Compilation Strategy
**Approach:** Build from macOS host to Android target using ARM/ARM64 architecture
**Dependencies:** Chromium and Node.js both have established Android support
**Build System:** GN/Ninja already supports `target_os = "android"`

## Milestone-Based Implementation Plan

### Milestone 1: Node.js "Hello World" on Android
**Timeline: 2-3 weeks**
**Goal: Get basic Electron app running Node.js code on Android, outputting to logcat**

#### Success Criteria
- ✅ APK builds successfully
- ✅ App installs and launches on Android device/emulator  
- ✅ "Hello from Node.js on Android!" appears in logcat
- ✅ Basic Node.js environment initializes without crashing

#### Technical Implementation

##### 1.1 Android App Shell Setup
```
shell/android/
├── app/
│   ├── src/main/
│   │   ├── AndroidManifest.xml
│   │   ├── java/org/electronjs/
│   │   │   └── ElectronActivity.java
│   │   └── jni/
│   │       └── electron_jni.cpp
│   └── build.gradle
└── gradle/
```

##### 1.2 Minimal Android Activity
```java
// shell/android/app/src/main/java/org/electronjs/ElectronActivity.java
public class ElectronActivity extends Activity {
    static {
        System.loadLibrary("electron");
    }
    
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        
        // Initialize Node.js and run Hello World
        runNodeHelloWorld();
    }
    
    private native void runNodeHelloWorld();
}
```

##### 1.3 Node.js Integration Layer
```cpp
// shell/android/app/src/main/jni/electron_jni.cpp
#include <jni.h>
#include <android/log.h>
#include "shell/app/node_main.h"
#include "shell/common/node_bindings.h"

extern "C" JNIEXPORT void JNICALL
Java_org_electronjs_ElectronActivity_runNodeHelloWorld(JNIEnv *env, jobject thiz) {
    // Initialize Node.js using Electron's existing integration
    // This uses the same entry point as desktop Electron
    electron::NodeMain();
    
    __android_log_print(ANDROID_LOG_INFO, "Electron", "Hello from Node.js on Android!");
}
```

##### 1.4 Build System Changes
```gn
# Add to BUILD.gn
if (is_android) {
  android_library("electron_java") {
    java_files = [ "shell/android/app/src/main/java/org/electronjs/ElectronActivity.java" ]
  }
  
  shared_library("electron_android") {
    sources = [
      "shell/android/app/src/main/jni/electron_jni.cpp",
      "shell/app/electron_main_android.cc",
    ]
    deps = [
      ":electron_lib",
      "//third_party/electron_node:libnode",
    ]
  }
  
  android_apk("electron_apk") {
    apk_name = "Electron"
    android_manifest = "shell/android/app/src/main/AndroidManifest.xml"
    java_files = [ ":electron_java" ]
    shared_libraries = [ ":electron_android" ]
  }
}
```

##### 1.5 Android Node.js Platform Bindings
```cpp
// shell/common/node_bindings_android.cc (create new file)
#include "shell/common/node_bindings.h"
#include <android/looper.h>

class NodeBindingsAndroid : public NodeBindings {
 public:
  NodeBindingsAndroid(BrowserEnvironment env) : NodeBindings(env) {}
  
  void PollEvents() override {
    // Use Android's ALooper for event integration
    // Similar to Linux epoll but Android-native
  }
};

// Factory method
std::unique_ptr<NodeBindings> NodeBindings::Create(BrowserEnvironment env) {
  return std::make_unique<NodeBindingsAndroid>(env);
}
```

##### 1.6 Required Files to Create
- `shell/android/app/src/main/AndroidManifest.xml` - Android app manifest
- `shell/android/app/build.gradle` - Android build configuration  
- `shell/android/gradle/wrapper/gradle-wrapper.properties` - Gradle setup
- `shell/common/node_bindings_android.cc` - Android Node.js event loop integration
- `patches/node/android_*.patch` - Android-specific Node.js patches
- Update `filenames.gni` to include Android-specific sources

---

### Milestone 2: Chromium WebView "Hello World"
**Timeline: 2-3 weeks**
**Goal: Display simple HTML page using Chromium's Android WebView**

#### Success Criteria
- ✅ HTML page renders in WebView
- ✅ Basic CSS and JavaScript work
- ✅ Node.js runs concurrently with WebView
- ✅ App responds to Android lifecycle events

#### Technical Implementation

##### 2.1 WebView Integration
```java
// Enhanced ElectronActivity.java
public class ElectronActivity extends Activity {
    private WebView webView;
    
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        
        // Initialize Chromium WebView
        webView = new WebView(this);
        setContentView(webView);
        
        // Load simple HTML
        webView.loadData(getHelloWorldHTML(), "text/html", "UTF-8");
        
        // Also initialize Node.js in background
        initializeNodeJS();
    }
    
    private String getHelloWorldHTML() {
        return "<html><body><h1>Hello from Chromium on Android!</h1>" +
               "<script>console.log('JavaScript works!');</script></body></html>";
    }
}
```

##### 2.2 Native Bridge Expansion
```cpp
// electron_jni.cpp expanded
#include "content/public/browser/web_contents.h"
#include "shell/browser/native_window_android.h"

extern "C" JNIEXPORT void JNICALL
Java_org_electronjs_ElectronActivity_initializeNodeJS(JNIEnv *env, jobject thiz) {
    // Initialize Node.js runtime
    // Set up basic Electron APIs bridge
    // Prepare for WebView <-> Node.js communication
}
```

##### 2.3 Basic Window Implementation
```cpp
// shell/browser/native_window_android.cc
class NativeWindowAndroid : public NativeWindow {
 public:
  NativeWindowAndroid(const gin_helper::Dictionary& options);
  
  // Basic window operations for Android
  void Show() override;
  void Hide() override;
  void SetTitle(const std::string& title) override;
  
 private:
  // Reference to Java WebView through JNI
  base::android::ScopedJavaGlobalRef<jobject> java_webview_;
};
```

---

### Milestone 3: Process-Based IPC Architecture
**Timeline: 3-4 weeks**
**Goal: Establish secure IPC between WebView and Node.js service processes while maintaining Electron's security model**

#### Success Criteria
- ✅ Main process runs as Android Service
- ✅ Renderer integrates with WebView  
- ✅ Mojo IPC works across Android processes
- ✅ Security boundaries maintained (process isolation)
- ✅ Context isolation functional
- ✅ `ipcRenderer.invoke()` and `ipcMain.handle()` work correctly

#### Security Analysis and Architecture Decision

**CRITICAL:** Initial threading approach would have **compromised Electron's security model**.

**Background:** During design discussions, a thread-based IPC approach was initially considered where Node.js and Chromium would run in the same Android process but different threads. This was **rejected** after detailed analysis of Electron's security architecture.

**Why Threading Was Considered:**
- Seemed simpler to implement on Android
- Avoided Android's Service complexity
- Appeared to align with Android's threading model

**Why Threading Was Rejected:**

##### Electron Security Dependencies on Process Isolation
- **Memory Isolation**: Process boundaries prevent renderer memory corruption from affecting main process
- **Sandbox Integrity**: Chromium's sandbox uses process-level restrictions that threads cannot replicate
- **Context Isolation**: V8 isolates assume process boundaries for maximum security  
- **Crash Isolation**: One process crash doesn't bring down entire application
- **Mojo IPC**: Electron uses Chromium's cross-process communication system

##### Android Process-Based Solution
Instead of threads, use Android's lightweight process model with Services:

```xml
<!-- AndroidManifest.xml -->
<application android:process=":main">
  <!-- Main process service -->
  <service android:name="ElectronMainService" 
           android:process=":main"
           android:exported="false" />
           
  <!-- Renderer processes -->
  <service android:name="ElectronRendererService"
           android:process=":renderer"
           android:exported="false" />
</application>
```

#### Technical Implementation

##### 3.1 Process Architecture
```
Android App
├── Main Activity (UI Thread)
│   └── WebView (displays content)
├── Main Process Service (:main)
│   ├── Node.js Runtime  
│   ├── Electron Main APIs
│   └── Mojo IPC Handler
└── Renderer Process Service (:renderer)
    ├── Chromium Renderer
    ├── V8 Context (isolated)
    └── Mojo IPC Bridge
```

##### 3.2 Main Process Service
```java
// ElectronMainService.java
public class ElectronMainService extends Service {
    @Override
    public void onCreate() {
        super.onCreate();
        // Initialize Node.js in this process
        initializeNodeJS();
        setupMojoIPC();
    }
    
    @Override
    public IBinder onBind(Intent intent) {
        // Return Binder for IPC with main activity
        return new ElectronMainBinder();
    }
    
    private native void initializeNodeJS();
    private native void setupMojoIPC();
}
```

##### 3.3 Secure IPC Flow (Preserving Electron Model)
```javascript
// Renderer (in WebView) - same as desktop Electron
ipcRenderer.invoke('fs-read-file', {path: '/sdcard/test.txt'})

// ↓ Mojo IPC (cross-process, same as desktop)

// Main Process Service
ipcMain.handle('fs-read-file', async (event, data) => {
  const fs = require('fs');
  return await fs.promises.readFile(data.path, 'utf8');
});

// ↓ Response via Mojo (maintains security boundaries)

// Back to WebView renderer
```

##### 3.4 Context Isolation Preservation
```cpp
// shell/renderer/preload_realm_context_android.cc
class PreloadRealmContextAndroid : public PreloadRealmContext {
 public:
  // Maintain isolated world execution (WorldID: ISOLATED_WORLD_ID=999)
  // Separate from main world (WorldID: MAIN_WORLD_ID=0)
  void SetupContextBridge() {
    // Same security model as desktop
    // contextBridge.exposeInMainWorld() works identically
  }
};
```

##### 3.5 Android Service Lifecycle Management
```cpp
// shell/browser/android/electron_service_manager.cc
class ElectronServiceManager {
 public:
  void StartMainProcessService();
  void StartRendererProcessService();
  void HandleServiceConnection();
  
 private:
  // Manage service lifecycle
  // Handle process termination and restart
  // Maintain IPC connection health
};
```

##### 3.6 Process Optimization for Android
```cpp
// Optimize process startup while maintaining security
class ElectronProcessOptimizer {
  void OptimizeForAndroid() {
    // Pre-warm V8 isolates in separate processes
    // Share read-only resources between processes (fonts, images)
    // Use Android's optimized process spawning
    // Implement process pooling for renderer processes
  }
};
```

#### Security Benefits Maintained
- ✅ **Process Isolation**: Renderer crashes don't affect main process
- ✅ **Memory Safety**: Separate address spaces prevent cross-contamination
- ✅ **Sandbox Integrity**: Process-level sandboxing preserved
- ✅ **Context Isolation**: V8 isolated worlds work as designed
- ✅ **Mojo IPC**: Same secure communication as desktop Electron

#### Android-Specific Optimizations
- ✅ **Service Architecture**: Uses Android's native process management
- ✅ **Binder IPC**: Leverages Android's optimized inter-process communication
- ✅ **Lifecycle Integration**: Proper Android service lifecycle handling
- ✅ **Resource Efficiency**: Optimized for mobile constraints

---

### Milestone 4: File System and Basic APIs
**Timeline: 3-4 weeks**
**Goal: Implement basic Electron APIs (fs, path, app) for Android**

#### Success Criteria
- ✅ Basic file system operations work
- ✅ Android permission handling integrated
- ✅ App lifecycle APIs functional
- ✅ Path utilities work correctly

#### Technical Implementation

##### 4.1 Android-Specific File System
```cpp
// shell/common/platform_util_android.cc
- Handle Android storage permissions
- Integrate with Storage Access Framework
- Support Android content URIs
- Manage scoped storage compliance
```

##### 4.2 Basic API Implementations
```cpp
// shell/browser/api/electron_api_app_android.cc
- App lifecycle events (onPause, onResume, onDestroy)
- Android-specific app information
- Permission management
- Background/foreground state handling
```

---

## Node.js Android Integration Strategy

### Leveraging Existing Electron Node.js Infrastructure

Instead of using Node.js for Mobile, we'll integrate the proven Android Node.js changes with Electron's existing Node.js patch system:

#### Current Electron Node.js Integration
- **51 patches** applied to Node.js for Electron compatibility
- **GN build system** replaces Node.js GYP (via `build_add_gn_build_files.patch`)
- **Static library**: Built as `//third_party/electron_node:libnode`
- **Platform bindings**: Linux/Mac/Windows event loop integration
- **Runtime integration**: `electron::NodeMain()` entry point

#### Android Node.js Changes to Integrate
From the [nodejs/node android-build branch](https://github.com/nodejs/node/compare/main...shivramk:node:android-build):

1. **Build System Fixes**:
   - Fixed host compiler environment variables (GCC-12 vs Android NDK)
   - Resolved ARM64 Android → x86_64 host execution issues
   - Added Android NDK cpufeatures support

2. **V8 Integration**:
   - Updated trap-handler logic for Android
   - Disabled trap handling for Android builds

3. **Cross-compilation improvements**:
   - Better Android toolchain configuration
   - Build compatibility enhancements

#### Integration Approach

##### Step 1: Extract Android Node.js Changes as Patches
```bash
# Create Android-specific patches from the android-build branch
git format-patch main...shivramk:node:android-build --output-directory patches/node/

# Rename with android_ prefix following Electron conventions
mv 0001-*.patch patches/node/android_fix_host_compiler_env.patch
mv 0002-*.patch patches/node/android_update_v8_trap_handler.patch
mv 0003-*.patch patches/node/android_add_ndk_cpufeatures.patch
```

##### Step 2: Extend Electron's Node.js Build Configuration
```gn
# In BUILD.gn, extend existing Node.js integration
if (is_android) {
  # Node.js already supports Android via our GN build files
  # Just need to ensure Android-specific patches are applied
  deps += [
    "//third_party/electron_node:libnode",  # Same as other platforms
  ]
}
```

##### Step 3: Create Android Platform Bindings
```cpp
// shell/common/node_bindings_android.cc
// Follow same pattern as node_bindings_linux.cc but for Android
class NodeBindingsAndroid : public NodeBindings {
  void PollEvents() override {
    // Integrate with Android's message loop
    // Use ALooper_pollAll() or similar Android event APIs
  }
};
```

##### Step 4: Update Entry Point
```cpp
// shell/app/electron_main_android.cc
int main(int argc, char* argv[]) {
  // Check if running as Node.js process
  if (base::CommandLine::ForCurrentProcess()->HasSwitch("type")) {
    return content::ContentMain(argc, argv, &electron::ElectronMainDelegate());
  }
  
  // Run as Node.js if ELECTRON_RUN_AS_NODE is set
  if (IsEnvSet("ELECTRON_RUN_AS_NODE")) {
    return electron::NodeMain();  // Same as desktop platforms
  }
  
  // Regular Android app flow
  return ElectronAndroidMain();
}
```

### Benefits of This Approach

✅ **Consistency**: Uses same Node.js integration as desktop Electron
✅ **Maintainability**: Android changes are patches, easy to update with new Node.js versions
✅ **Testing**: Can run existing Electron Node.js tests on Android
✅ **Compatibility**: All Electron's Node.js customizations work on Android
✅ **Native modules**: Existing node-gyp modifications apply to Android

---

## Long-term Architecture Plan

### Phase 5: Complete Platform Integration (8-12 weeks)
**Components:**
- Full Electron API compatibility
- Android UI/UX adaptations (Material Design dialogs, navigation)
- Hardware integration (camera, sensors, GPS)
- Performance optimizations (memory management, battery)
- Security model integration

### Phase 6: Packaging and Distribution (4-6 weeks)
**Components:**
- APK generation and signing
- Google Play Store compatibility
- Auto-update mechanisms
- Developer tooling and documentation

## Development Environment Setup

### Required Dependencies
```bash
# Android SDK and NDK
export ANDROID_SDK_ROOT=/path/to/android-sdk
export ANDROID_NDK_ROOT=/path/to/android-ndk

# Add to .gclient file
target_os = ['linux', 'android']

# GN build arguments
target_os = "android"
target_cpu = "arm64"  # or "arm" for 32-bit
is_debug = true
is_component_build = true
is_clang = true
symbol_level = 1
```

### Build Commands
```bash
# Generate build files
gn gen out/android --args='target_os="android" target_cpu="arm64"'

# Build Electron for Android
autoninja -C out/android electron_apk

# Install on device
adb install out/android/apks/Electron.apk
```

## File Structure Overview

### New Files to Create
```
shell/android/                           # Android app wrapper
├── app/src/main/
│   ├── AndroidManifest.xml             # App manifest
│   ├── java/org/electronjs/            # Java source files
│   └── jni/                            # JNI bridge code
├── gradle/                             # Gradle configuration
└── build.gradle                        # Build script

shell/browser/android/                   # Android-specific browser code
├── browser_android.cc                  # Browser implementation
├── native_window_android.cc            # Window management
└── electron_application_android.cc     # App lifecycle

shell/common/android/                    # Android utilities
├── platform_util_android.cc           # Platform utilities
├── application_info_android.cc        # App information
└── jni_util.cc                        # JNI helpers
```

### Files to Modify
```
BUILD.gn                                # Add Android build targets
filenames.gni                          # Add lib_sources_android
buildflags/buildflags.gni              # Android build flags
shell/app/electron_main_android.cc     # Expand Android entry point
```

## Technical Challenges and Solutions

### Challenge 1: Application Model Mismatch
**Problem:** Desktop window model vs Android Activity model
**Solution:** Create abstraction layer mapping Electron windows to Android Activities/Fragments

### Challenge 2: Node.js Android Integration
**Problem:** Integrating Android Node.js build changes with Electron's patch system
**Solution:** Apply Android Node.js changes as patches in `/patches/node/`, leverage Electron's existing GN build system, and create Android-specific event loop bindings

### Challenge 3: IPC and Security Model
**Problem:** Maintaining Electron's security model on Android while enabling IPC
**Solution:** Use Android Services with separate processes to preserve Electron's process isolation, leverage Mojo IPC system, and maintain context isolation boundaries

### Challenge 4: Android Security Model
**Problem:** Permissions, sandboxing, and security restrictions
**Solution:** Implement Android permission management and follow security best practices

## Areas Requiring Further Investigation

### 1. Android-Specific Implementation Details
**Areas needing clarification:**
- **Service lifecycle integration**: How Android Services integrate with Electron's process lifecycle
- **Memory management**: Android's memory constraints vs Electron's resource usage  
- **Background restrictions**: How Android's background execution limits affect Node.js runtime
- **Permission model**: Integration between Android permissions and Electron's security model

### 2. Performance Considerations
**Unknown factors:**
- **Process startup overhead**: Impact of separate processes on Android performance
- **Memory usage**: Multiple processes vs single process memory consumption
- **Battery impact**: Background Node.js service effects on battery life
- **Resource sharing**: Optimization opportunities between processes

### 3. Development Workflow
**Unclear aspects:**
- **Debugging strategy**: How to debug across multiple Android processes
- **Hot reload**: Developer experience for Electron app development on Android
- **Testing approach**: Unit and integration testing strategies for Android-specific code

### 4. Packaging and Distribution
**Unresolved questions:**
- **APK size**: Impact of bundling Node.js and Chromium in Android app
- **App store compliance**: Google Play Store policies for apps with embedded runtimes
- **Update mechanisms**: How Electron's auto-update integrates with Android app updates
- **Native modules**: Building and packaging Node.js native modules for Android

## Resource Requirements

### Development Tools
- Android Studio (debugging, profiling, emulator)
- Android SDK/NDK (API level 26+ recommended)
- Cross-compilation toolchain
- Android devices for testing (ARM/ARM64 recommended)

### Knowledge Requirements
- **Android development**: Services, Binder IPC, lifecycle management
- **Chromium internals**: Mojo IPC, content API, sandbox architecture
- **Node.js integration**: Event loop, libuv, native modules
- **Electron architecture**: Main/renderer processes, security model, build system

### Estimated Timeline
- **Milestone 1-4:** 10-14 weeks (updated for process-based IPC complexity)
- **Complete implementation:** 23-33 weeks total
- **Production ready:** Add 8-12 weeks for testing, optimization, documentation

## Success Metrics

### Milestone Completion
- Each milestone has clear, testable success criteria
- Progressive complexity building toward full Android support
- Concrete deliverables at each stage

### Technical Validation
- APK builds and installs successfully
- Core Electron APIs function correctly
- Performance meets Android app standards
- **Security requirements satisfied (process isolation maintained)**
- **Mojo IPC functions correctly across Android processes**
- **Context isolation and sandbox integrity preserved**

### Community Readiness
- Documentation and examples available
- Developer tooling functional
- Community can build and test Android Electron apps

## Potential Risks and Mitigation Strategies

### High-Risk Areas

#### 1. Android Service Stability
**Risk:** Android may terminate background services, disrupting Node.js runtime
**Mitigation:** Implement foreground service with persistent notification, service restart mechanisms

#### 2. Memory Constraints  
**Risk:** Multiple processes may exceed Android memory limits
**Mitigation:** Implement process pooling, lazy loading, memory monitoring

#### 3. Performance Impact
**Risk:** Process separation overhead may make Android apps too slow
**Mitigation:** Benchmark early, optimize IPC, consider hybrid approaches for performance-critical paths

#### 4. Security Model Complexity
**Risk:** Android's permission model may conflict with Electron's security assumptions
**Mitigation:** Design permission bridge layer, extensive security testing

### Medium-Risk Areas

#### 1. Build System Complexity
**Risk:** Android build integration may be more complex than anticipated
**Mitigation:** Start with minimal builds, incrementally add complexity

#### 2. Node.js Native Modules
**Risk:** Existing native modules may not compile for Android
**Mitigation:** Focus on core functionality first, address native modules in later phases

## Key References and External Dependencies

### Critical External Resources
- **Chromium Android docs**: https://chromium.googlesource.com/chromium/src/+/main/docs/android_build_instructions.md
- **Android Node.js changes**: https://github.com/nodejs/node/compare/main...shivramk:node:android-build
- **Electron IPC documentation**: https://www.electronjs.org/docs/latest/tutorial/ipc
- **Android processes/threads guide**: https://developer.android.com/guide/components/processes-and-threads
- **Android Services guide**: https://developer.android.com/develop/background-work/services

### Key Codebase Analysis Results
- **Electron's Node.js patches**: 51 patches in `/patches/node/` directory
- **Platform-specific bindings**: Pattern established in `shell/common/node_bindings_*.cc` files
- **Build system**: GN configuration supports Android via `target_os = "android"`
- **Security architecture**: Mojo IPC, process isolation, context isolation all depend on separate processes

## Next Steps

1. **Begin Milestone 1**: Set up Android build environment and create minimal Node.js APK
2. **Extract Node.js patches**: Apply android-build branch changes as Electron patches
3. **Create Android platform bindings**: Implement `node_bindings_android.cc`
4. **Establish development workflow**: Build, install, debug cycle for Android
5. **Community coordination**: Work with Electron maintainers on architecture decisions
6. **Documentation**: Maintain detailed progress documentation throughout implementation

## Decision Points for Future Review

### Architecture Decisions to Validate
1. **Service vs Activity model**: Confirm Android Services are the right approach for main process
2. **IPC transport**: Validate Mojo works efficiently across Android processes  
3. **Memory management**: Confirm multiple processes don't exceed Android constraints
4. **Performance benchmarks**: Measure vs native Android apps and desktop Electron

### Implementation Decisions to Make
1. **Foreground vs background services**: Determine service types for different use cases
2. **Process lifecycle**: How to handle Android killing and restarting processes
3. **Native module strategy**: Which modules to prioritize for Android compatibility
4. **Testing approach**: Unit, integration, and end-to-end testing strategies

---

*This plan provides a structured approach to implementing Android support in Electron through incremental, testable milestones that build systematically toward full platform support.*