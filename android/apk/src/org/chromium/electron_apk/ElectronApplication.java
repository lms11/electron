// Copyright 2012 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

package org.chromium.electron_apk;

import android.app.Application;
import android.content.Context;

import org.chromium.base.ApplicationStatus;
import org.chromium.base.CommandLine;
import org.chromium.base.ContextUtils;
import org.chromium.base.PathUtils;
import org.chromium.base.library_loader.LibraryLoader;
import org.chromium.base.library_loader.LibraryProcessType;
import org.chromium.electron_shell.AssetExtractor;
import org.chromium.ui.base.ResourceBundle;

/**
 * Entry point for the content shell application.  Handles initialization of information that needs
 * to be shared across the main activity and the child services created.
 */
public class ElectronApplication extends Application {
    public static final String COMMAND_LINE_FILE = "/data/local/tmp/electron-command-line";
    private static final String PRIVATE_DATA_DIRECTORY_SUFFIX = "electron";
    
    private static AssetExtractor sAssetExtractor;

    @Override
    protected void attachBaseContext(Context base) {
        super.attachBaseContext(base);
        boolean isBrowserProcess = !ContextUtils.getProcessName().contains(":");
        ContextUtils.initApplicationContext(this);
        ResourceBundle.setNoAvailableLocalePaks();
        LibraryLoader.getInstance()
                .setLibraryProcessType(
                        isBrowserProcess
                                ? LibraryProcessType.PROCESS_BROWSER
                                : LibraryProcessType.PROCESS_CHILD);
        if (isBrowserProcess) {
            PathUtils.setPrivateDataDirectorySuffix(PRIVATE_DATA_DIRECTORY_SUFFIX);
            ApplicationStatus.initialize(this);
            // Initialize and extract assets
            sAssetExtractor = new AssetExtractor(this);
            sAssetExtractor.extractAssetsIfNeeded();
        }
    }

    public void initCommandLine() {
        if (!CommandLine.isInitialized()) {
            CommandLine.initFromFile(COMMAND_LINE_FILE);
        }
    }
    
    /**
     * Gets the AssetExtractor instance used by the application.
     * 
     * @return The AssetExtractor, or null if called from a non-browser process
     */
    public static AssetExtractor getAssetExtractor() {
        return sAssetExtractor;
    }
}