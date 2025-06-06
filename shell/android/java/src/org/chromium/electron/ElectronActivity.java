// Copyright 2024 The Electron Authors. All rights reserved.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

package org.chromium.electron;

import android.app.Activity;
import android.content.Intent;
import android.graphics.Color;
import android.os.Build;
import android.os.Bundle;
import android.view.View;
import android.view.ViewGroup;
import android.view.WindowManager;
import android.widget.FrameLayout;

import androidx.annotation.CallSuper;

import org.chromium.base.ActivityState;
import org.chromium.base.ApplicationStatus;
import org.chromium.base.Log;
import org.chromium.base.supplier.ObservableSupplierImpl;
import org.chromium.content_public.browser.WebContents;
import org.chromium.ui.base.ActivityWindowAndroid;
import org.chromium.ui.base.IntentRequestTracker;
import org.chromium.ui.base.WindowAndroid;

/**
 * Main activity for Electron applications on Android.
 * Each instance represents a BrowserWindow in Electron.
 */
public class ElectronActivity extends Activity implements ApplicationStatus.ActivityStateListener {
    private static final String TAG = "ElectronActivity";
    
    private ActivityWindowAndroid mWindowAndroid;
    private ElectronWindow mElectronWindow;
    private FrameLayout mContentView;
    private WebContents mWebContents;
    private long mNativeElectronActivity;
    private final ObservableSupplierImpl<Boolean> mBackPressedSupplier = 
            new ObservableSupplierImpl<>();
    
    @Override
    @CallSuper
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        
        // Initialize the application if needed.
        ElectronApplication app = (ElectronApplication) getApplication();
        app.initializeBrowserProcess();
        
        // Create the window.
        mWindowAndroid = new ActivityWindowAndroid(this, /* listenToActivityState= */ true,
                IntentRequestTracker.createFromActivity(this),
                /* insetObserver= */ null,
                /* trackOcclusion= */ false);
        
        // Set up the content view.
        mContentView = new FrameLayout(this);
        mContentView.setLayoutParams(new FrameLayout.LayoutParams(
                ViewGroup.LayoutParams.MATCH_PARENT,
                ViewGroup.LayoutParams.MATCH_PARENT));
        setContentView(mContentView);
        
        // Register for activity state changes.
        ApplicationStatus.registerStateListenerForActivity(this, this);
        
        // Initialize native components.
        mNativeElectronActivity = nativeInit();
        
        // Create the Electron window wrapper.
        mElectronWindow = new ElectronWindow(this, mWindowAndroid);
        
        // Handle intent to determine what to load.
        handleIntent(getIntent());
    }
    
    @Override
    protected void onStart() {
        super.onStart();
        if (mNativeElectronActivity != 0) {
            nativeOnStart(mNativeElectronActivity);
        }
    }
    
    @Override
    protected void onResume() {
        super.onResume();
        if (mNativeElectronActivity != 0) {
            nativeOnResume(mNativeElectronActivity);
        }
    }
    
    @Override
    protected void onPause() {
        super.onPause();
        if (mNativeElectronActivity != 0) {
            nativeOnPause(mNativeElectronActivity);
        }
    }
    
    @Override
    protected void onStop() {
        super.onStop();
        if (mNativeElectronActivity != 0) {
            nativeOnStop(mNativeElectronActivity);
        }
    }
    
    @Override
    protected void onDestroy() {
        if (mElectronWindow != null) {
            mElectronWindow.destroy();
            mElectronWindow = null;
        }
        
        if (mWebContents != null) {
            mWebContents.destroy();
            mWebContents = null;
        }
        
        if (mWindowAndroid != null) {
            mWindowAndroid.destroy();
            mWindowAndroid = null;
        }
        
        if (mNativeElectronActivity != 0) {
            nativeOnDestroy(mNativeElectronActivity);
            mNativeElectronActivity = 0;
        }
        
        ApplicationStatus.unregisterActivityStateListener(this);
        super.onDestroy();
    }
    
    @Override
    public void onBackPressed() {
        if (mElectronWindow != null && mElectronWindow.handleBackPressed()) {
            return;
        }
        
        mBackPressedSupplier.set(true);
        if (!mBackPressedSupplier.get()) {
            return;
        }
        
        super.onBackPressed();
    }
    
    @Override
    public void onActivityStateChange(Activity activity, @ActivityState int newState) {
        if (activity == this && mNativeElectronActivity != 0) {
            nativeOnActivityStateChange(mNativeElectronActivity, newState);
        }
    }
    
    @Override
    protected void onNewIntent(Intent intent) {
        super.onNewIntent(intent);
        handleIntent(intent);
    }
    
    private void handleIntent(Intent intent) {
        // Handle the intent to determine what URL or file to load.
        String url = intent.getStringExtra("url");
        if (url == null) {
            // Default to loading the app's main entry point.
            url = "file:///android_asset/default_app/index.html";
        }
        
        if (mElectronWindow != null) {
            mElectronWindow.loadUrl(url);
        }
    }
    
    /**
     * Get the content view for adding web contents.
     */
    public ViewGroup getContentView() {
        return mContentView;
    }
    
    /**
     * Get the WindowAndroid instance.
     */
    public WindowAndroid getWindowAndroid() {
        return mWindowAndroid;
    }
    
    /**
     * Get the associated ElectronWindow.
     */
    public ElectronWindow getElectronWindow() {
        return mElectronWindow;
    }
    
    /**
     * Set the WebContents for this activity.
     */
    public void setWebContents(WebContents webContents) {
        mWebContents = webContents;
    }
    
    /**
     * Enable or disable fullscreen mode.
     */
    public void setFullscreen(boolean fullscreen) {
        View decorView = getWindow().getDecorView();
        if (fullscreen) {
            int uiOptions = View.SYSTEM_UI_FLAG_FULLSCREEN
                    | View.SYSTEM_UI_FLAG_HIDE_NAVIGATION
                    | View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY;
            decorView.setSystemUiVisibility(uiOptions);
            
            getWindow().addFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN);
        } else {
            decorView.setSystemUiVisibility(View.SYSTEM_UI_FLAG_VISIBLE);
            getWindow().clearFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN);
        }
    }
    
    // Native methods
    private native long nativeInit();
    private native void nativeOnStart(long nativeElectronActivity);
    private native void nativeOnResume(long nativeElectronActivity);
    private native void nativeOnPause(long nativeElectronActivity);
    private native void nativeOnStop(long nativeElectronActivity);
    private native void nativeOnDestroy(long nativeElectronActivity);
    private native void nativeOnActivityStateChange(long nativeElectronActivity, int newState);
}