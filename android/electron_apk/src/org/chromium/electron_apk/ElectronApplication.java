// Copyright 2012 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

package org.chromium.electron_apk;

import android.app.Application;
import android.content.Context;
import android.content.res.AssetManager;
import android.util.Log;

import org.chromium.base.ApplicationStatus;
import org.chromium.base.CommandLine;
import org.chromium.base.ContextUtils;
import org.chromium.base.PathUtils;
import org.chromium.base.library_loader.LibraryLoader;
import org.chromium.base.library_loader.LibraryProcessType;
import org.chromium.ui.base.ResourceBundle;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

/**
 * Entry point for the content shell application.  Handles initialization of information that needs
 * to be shared across the main activity and the child services created.
 */
public class ElectronApplication extends Application {
    public static final String COMMAND_LINE_FILE = "/data/local/tmp/electron-command-line";
    private static final String PRIVATE_DATA_DIRECTORY_SUFFIX = "electron";
    private static final String TAG = "ElectronApplication";
    private static final String ASSETS_EXTRACTED_MARKER = ".assets_extracted";

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
            // Extract assets after initialization
            extractAssetsIfNeeded();
        }
    }

    public void initCommandLine() {
        if (!CommandLine.isInitialized()) {
            CommandLine.initFromFile(COMMAND_LINE_FILE);
        }
    }

    private void extractAssetsIfNeeded() {
        try {
            File filesDir = getFilesDir();
            File markerFile = new File(filesDir, ASSETS_EXTRACTED_MARKER);
            
            // Check if assets have already been extracted
            if (markerFile.exists()) {
                Log.d(TAG, "Assets already extracted, skipping");
                return;
            }
            
            Log.d(TAG, "Extracting assets to: " + filesDir.getAbsolutePath());
            
            // Extract assets
            AssetManager assetManager = getAssets();
            File assetsDir = new File(filesDir, "assets");
            copyAssetFolder(assetManager, "", assetsDir.getAbsolutePath());
            
            // Create marker file to indicate extraction is complete
            markerFile.createNewFile();
            Log.d(TAG, "Assets extraction complete");
            
        } catch (IOException e) {
            Log.e(TAG, "Failed to extract assets", e);
        }
    }
    
    private void copyAssetFolder(AssetManager assetManager, String assetPath, String targetPath) throws IOException {
        String[] assets = assetManager.list(assetPath);
        
        if (assets == null || assets.length == 0) {
            // It's a file, not a directory
            copyAssetFile(assetManager, assetPath, targetPath);
        } else {
            // It's a directory
            File targetDir = new File(targetPath);
            if (!targetDir.exists()) {
                targetDir.mkdirs();
            }
            
            for (String asset : assets) {
                String assetFilePath = assetPath.isEmpty() ? asset : assetPath + "/" + asset;
                String targetFilePath = targetPath + "/" + asset;
                copyAssetFolder(assetManager, assetFilePath, targetFilePath);
            }
        }
    }
    
    private void copyAssetFile(AssetManager assetManager, String assetPath, String targetPath) throws IOException {
        File targetFile = new File(targetPath);
        File parentDir = targetFile.getParentFile();
        if (!parentDir.exists()) {
            parentDir.mkdirs();
        }
        
        try (InputStream in = assetManager.open(assetPath);
             OutputStream out = new FileOutputStream(targetFile)) {
            byte[] buffer = new byte[1024];
            int read;
            while ((read = in.read(buffer)) != -1) {
                out.write(buffer, 0, read);
            }
            out.flush();
        }
    }
}