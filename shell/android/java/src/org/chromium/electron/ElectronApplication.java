// Copyright 2024 The Electron Authors. All rights reserved.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

package org.chromium.electron;

import android.app.Application;
import android.content.Context;
import android.content.res.Configuration;
import android.os.Build;
import android.os.Bundle;
import android.os.SystemClock;

import org.chromium.base.ApplicationState;
import org.chromium.base.ApplicationStatus;
import org.chromium.base.BundleUtils;
import org.chromium.base.CommandLine;
import org.chromium.base.ContextUtils;
import org.chromium.base.PathUtils;
import org.chromium.base.library_loader.LibraryLoader;
import org.chromium.base.library_loader.LibraryProcessType;
import org.chromium.base.library_loader.ProcessInitException;
import org.chromium.base.memory.MemoryPressureUma;
import org.chromium.base.task.AsyncTask;
import org.chromium.base.task.TaskTraits;
import org.chromium.content_public.browser.BrowserStartupController;

/**
 * Electron application class for Android.
 * Handles initialization of the Chromium content layer and Node.js runtime.
 */
public class ElectronApplication extends Application {
    private static final String TAG = "ElectronApplication";
    private static final String PRIVATE_DATA_DIRECTORY_SUFFIX = "electron";
    
    @Override
    protected void attachBaseContext(Context base) {
        super.attachBaseContext(base);
        ContextUtils.initApplicationContext(this);
        
        if (isBrowserProcess()) {
            // Note: SelectionPopupBackgroundThread initialization would go here
            // if needed for Android O+ text selection handling
            
            // JNI environment is set up automatically by Chromium
        }
    }
    
    @Override
    public void onCreate() {
        super.onCreate();
        
        // Initialize command line early.
        if (!CommandLine.isInitialized()) {
            CommandLine.initFromFile("/data/local/tmp/electron-command-line");
        }
        
        // Set up activity lifecycle callbacks.
        ApplicationStatus.initialize(this);
        
        // Initialize path utils with the private data directory.
        PathUtils.setPrivateDataDirectorySuffix(PRIVATE_DATA_DIRECTORY_SUFFIX);
        
        if (isBrowserProcess()) {
            // Register memory pressure callbacks.
            MemoryPressureUma.initializeForBrowser();
            ApplicationStatus.registerApplicationStateListener(
                    new ApplicationStatus.ApplicationStateListener() {
                        @Override
                        public void onApplicationStateChange(int newState) {
                            if (newState == ApplicationState.HAS_RUNNING_ACTIVITIES) {
                                onApplicationHasRunningActivities();
                            }
                        }
                    });
        }
    }
    
    /**
     * Initialize the browser process asynchronously.
     */
    public void initializeBrowserProcess() {
        try {
            LibraryLoader.getInstance().ensureInitialized();
        } catch (ProcessInitException e) {
            throw new RuntimeException("Failed to load native library", e);
        }
        
        // Native resources are loaded automatically by LibraryLoader
        
        // Device-specific initialization handled by Chromium
        
        AsyncTask.THREAD_POOL_EXECUTOR.execute(new Runnable() {
            @Override
            public void run() {
                try {
                    LibraryLoader.getInstance().ensureInitialized();
                    
                    // Start the browser process.
                    BrowserStartupController.getInstance().startBrowserProcessesSync(
                            LibraryProcessType.PROCESS_BROWSER,
                            false,
                            false);
                    
                    // Initialize Electron-specific components.
                    nativeInitializeElectron();
                } catch (ProcessInitException e) {
                    throw new RuntimeException("Failed to start browser", e);
                }
            }
        });
    }
    
    private void onApplicationHasRunningActivities() {
        // Perform any initialization that requires an activity.
    }
    
    private boolean isBrowserProcess() {
        return !ContextUtils.getProcessName().contains(":");
    }
    
    @Override
    public void onConfigurationChanged(Configuration newConfig) {
        super.onConfigurationChanged(newConfig);
        // Handle configuration changes.
    }
    
    @Override
    public void onLowMemory() {
        super.onLowMemory();
        // Handle low memory conditions.
    }
    
    /**
     * Native method to initialize Electron-specific components.
     */
    private native void nativeInitializeElectron();
}
