// Copyright 2024 The Electron Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

package org.chromium.electron_shell;

import android.content.Context;
import android.content.res.AssetManager;
import android.util.Log;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

/**
 * Utility class for extracting assets from the APK to the filesystem.
 */
public class AssetExtractor {
    private static final String TAG = "AssetExtractor";
    private static final String ASSETS_EXTRACTED_MARKER = ".assets_extracted";
    private static final String ASSETS_DIR_NAME = "assets";
    
    private final Context mContext;
    private final File mTargetDir;
    
    /**
     * Creates an AssetExtractor that will extract to the default assets directory.
     * 
     * @param context The application context
     */
    public AssetExtractor(Context context) {
        this(context, new File(context.getFilesDir(), ASSETS_DIR_NAME));
    }
    
    /**
     * Creates an AssetExtractor that will extract to a custom directory.
     * 
     * @param context The application context
     * @param targetDir The directory to extract assets to
     */
    public AssetExtractor(Context context, File targetDir) {
        mContext = context;
        mTargetDir = targetDir;
    }
    
    /**
     * Extracts all assets to the target directory if they haven't been extracted yet.
     * 
     * @return true if extraction was successful or assets were already extracted, false on error
     */
    public boolean extractAssetsIfNeeded() {
        File markerFile = new File(mTargetDir.getParentFile(), ASSETS_EXTRACTED_MARKER);
        
        // Check if assets have already been extracted
        if (markerFile.exists()) {
            Log.d(TAG, "Assets already extracted, skipping");
            return true;
        }
        
        try {
            Log.d(TAG, "Extracting assets to: " + mTargetDir.getAbsolutePath());
            
            // Extract all assets
            AssetManager assetManager = mContext.getAssets();
            extractAssetFolder(assetManager, "", mTargetDir.getAbsolutePath());
            
            // Create marker file to indicate extraction is complete
            markerFile.createNewFile();
            Log.d(TAG, "Assets extraction complete");
            return true;
            
        } catch (IOException e) {
            Log.e(TAG, "Failed to extract assets", e);
            return false;
        }
    }
    
    /**
     * Gets the path to the extracted assets directory.
     * 
     * @return The absolute path to the assets directory
     */
    public String getAssetsPath() {
        return mTargetDir.getAbsolutePath();
    }
    
    /**
     * Gets the file URL for a specific asset file.
     * 
     * @param assetPath The relative path to the asset (e.g., "index.html")
     * @return The file:// URL to the extracted asset
     */
    public String getAssetFileUrl(String assetPath) {
        File assetFile = new File(mTargetDir, assetPath);
        return "file://" + assetFile.getAbsolutePath();
    }
    
    /**
     * Recursively copies an asset folder to the target path.
     */
    private void extractAssetFolder(AssetManager assetManager, String assetPath, 
                                    String targetPath) throws IOException {
        String[] assets = assetManager.list(assetPath);
        
        if (assets == null || assets.length == 0) {
            // It's a file, not a directory
            extractAssetFile(assetManager, assetPath, targetPath);
        } else {
            // It's a directory
            File targetDir = new File(targetPath);
            if (!targetDir.exists()) {
                targetDir.mkdirs();
            }
            
            for (String asset : assets) {
                String assetFilePath = assetPath.isEmpty() ? asset : assetPath + "/" + asset;
                String targetFilePath = targetPath + "/" + asset;
                extractAssetFolder(assetManager, assetFilePath, targetFilePath);
            }
        }
    }
    
    /**
     * Copies a single asset file to the target path.
     */
    private void extractAssetFile(AssetManager assetManager, String assetPath, 
                                  String targetPath) throws IOException {
        File targetFile = new File(targetPath);
        File parentDir = targetFile.getParentFile();
        if (!parentDir.exists()) {
            parentDir.mkdirs();
        }
        
        try (InputStream in = assetManager.open(assetPath);
             OutputStream out = new FileOutputStream(targetFile)) {
            byte[] buffer = new byte[8192];
            int read;
            while ((read = in.read(buffer)) != -1) {
                out.write(buffer, 0, read);
            }
            out.flush();
        }
    }
}