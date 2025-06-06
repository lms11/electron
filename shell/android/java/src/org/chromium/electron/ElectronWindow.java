// Copyright 2024 The Electron Authors. All rights reserved.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

package org.chromium.electron;

import android.content.Context;
import android.graphics.Color;
import android.graphics.Rect;
import android.view.View;
import android.view.ViewGroup;
import android.widget.FrameLayout;

import org.chromium.base.Log;
import org.jni_zero.CalledByNative;
import org.jni_zero.JNINamespace;
import org.jni_zero.NativeMethods;
import org.chromium.content_public.browser.LoadUrlParams;
import org.chromium.content_public.browser.NavigationController;
import org.chromium.content_public.browser.WebContents;
import org.chromium.content_public.browser.WebContentsObserver;
import org.chromium.content.browser.webcontents.EmptyInternalAccessDelegate;
import org.chromium.ui.base.ViewAndroidDelegate;
import org.chromium.ui.base.WindowAndroid;
import org.chromium.electron.ElectronWindowJni;

/**
 * Represents an Electron BrowserWindow on Android.
 * Manages the WebContents and provides window-like functionality.
 */
@JNINamespace("electron")
public class ElectronWindow {
    private static final String TAG = "ElectronWindow";
    
    private final ElectronActivity mActivity;
    private final WindowAndroid mWindowAndroid;
    private WebContents mWebContents;
    private View mContentView;
    private long mNativeElectronWindow;
    private boolean mIsFullscreen;
    private final Rect mBounds = new Rect();
    
    /**
     * Observer for WebContents events.
     */
    private class ElectronWebContentsObserver extends WebContentsObserver {
        public ElectronWebContentsObserver(WebContents webContents) {
            super(webContents);
        }
        
        @Override
        public void documentLoadedInPrimaryMainFrame(
                org.chromium.content_public.browser.Page page,
                org.chromium.content_public.browser.GlobalRenderFrameHostId rfhId,
                int rfhLifecycleState) {
            if (mNativeElectronWindow != 0) {
                ElectronWindowJni.get().onDocumentLoadedInMainFrame(
                        mNativeElectronWindow, ElectronWindow.this);
            }
        }
        
        @Override
        public void didFinishNavigationInPrimaryMainFrame(
                org.chromium.content_public.browser.NavigationHandle navigationHandle) {
            if (mNativeElectronWindow != 0) {
                ElectronWindowJni.get().onNavigationFinished(
                        mNativeElectronWindow, ElectronWindow.this);
            }
        }
        
        @Override
        public void titleWasSet(String title) {
            if (mNativeElectronWindow != 0) {
                ElectronWindowJni.get().onTitleChanged(
                        mNativeElectronWindow, ElectronWindow.this, title);
            }
        }
    }
    
    public ElectronWindow(ElectronActivity activity, WindowAndroid windowAndroid) {
        mActivity = activity;
        mWindowAndroid = windowAndroid;
        
        // Initialize native side.
        mNativeElectronWindow = ElectronWindowJni.get().init(this);
        
        // Set default bounds to full screen.
        mBounds.set(0, 0, 
                mActivity.getResources().getDisplayMetrics().widthPixels,
                mActivity.getResources().getDisplayMetrics().heightPixels);
    }
    
    /**
     * Create and attach WebContents to this window.
     */
    @CalledByNative
    public void createWebContents() {
        if (mWebContents != null) {
            Log.w(TAG, "WebContents already created");
            return;
        }
        
        // Create WebContents through native.
        mWebContents = ElectronWindowJni.get().createWebContents(
                mNativeElectronWindow, ElectronWindow.this);
        
        if (mWebContents == null) {
            Log.e(TAG, "Failed to create WebContents");
            return;
        }
        
        // Create content view.
        Context context = mActivity;
        mContentView = ContentViewRenderView.createContentViewRenderView(context);
        mContentView.setLayoutParams(new FrameLayout.LayoutParams(
                ViewGroup.LayoutParams.MATCH_PARENT,
                ViewGroup.LayoutParams.MATCH_PARENT));
        
        // Set up view hierarchy.
        ViewGroup contentViewParent = mActivity.getContentView();
        contentViewParent.addView(mContentView);
        
        // Initialize WebContents with view.
        mWebContents.setDelegates("Electron", 
                ViewAndroidDelegate.createBasicDelegate((ViewGroup) mContentView),
                new EmptyInternalAccessDelegate(), mWindowAndroid, 
                WebContents.createDefaultInternalsHolder());
        
        // Set up observer.
        new ElectronWebContentsObserver(mWebContents);
        
        // Notify native that setup is complete.
        if (mNativeElectronWindow != 0) {
            ElectronWindowJni.get().onWebContentsCreated(
                    mNativeElectronWindow, ElectronWindow.this, mWebContents);
        }
    }
    
    /**
     * Load a URL in this window.
     */
    public void loadUrl(String url) {
        if (mWebContents == null) {
            createWebContents();
        }
        
        if (mWebContents != null) {
            NavigationController navigationController = mWebContents.getNavigationController();
            navigationController.loadUrl(new LoadUrlParams(url));
        }
    }
    
    /**
     * Set the window bounds (position and size).
     */
    @CalledByNative
    public void setBounds(int x, int y, int width, int height) {
        mBounds.set(x, y, x + width, y + height);
        
        // On Android, we can't freely position windows, but we can resize the content view.
        if (mContentView != null) {
            ViewGroup.LayoutParams params = mContentView.getLayoutParams();
            params.width = width;
            params.height = height;
            mContentView.setLayoutParams(params);
        }
    }
    
    /**
     * Get the window bounds.
     */
    @CalledByNative
    public int[] getBounds() {
        return new int[] {
            mBounds.left, mBounds.top, 
            mBounds.width(), mBounds.height()
        };
    }
    
    /**
     * Show or hide the window.
     */
    @CalledByNative
    public void setVisible(boolean visible) {
        if (mContentView != null) {
            mContentView.setVisibility(visible ? View.VISIBLE : View.GONE);
        }
    }
    
    /**
     * Set fullscreen state.
     */
    @CalledByNative
    public void setFullscreen(boolean fullscreen) {
        mIsFullscreen = fullscreen;
        mActivity.setFullscreen(fullscreen);
    }
    
    /**
     * Check if window is fullscreen.
     */
    @CalledByNative
    public boolean isFullscreen() {
        return mIsFullscreen;
    }
    
    /**
     * Close the window (finish the activity).
     */
    @CalledByNative
    public void close() {
        mActivity.finish();
    }
    
    /**
     * Focus the window (bring to front).
     */
    @CalledByNative
    public void focus() {
        if (mContentView != null) {
            mContentView.requestFocus();
        }
    }
    
    /**
     * Set the window title.
     */
    @CalledByNative
    public void setTitle(String title) {
        mActivity.setTitle(title);
    }
    
    /**
     * Get the window title.
     */
    @CalledByNative
    public String getTitle() {
        CharSequence title = mActivity.getTitle();
        return title != null ? title.toString() : "";
    }
    
    /**
     * Handle back button press.
     */
    public boolean handleBackPressed() {
        if (mWebContents != null && mWebContents.getNavigationController().canGoBack()) {
            mWebContents.getNavigationController().goBack();
            return true;
        }
        return false;
    }
    
    /**
     * Clean up resources.
     */
    public void destroy() {
        if (mNativeElectronWindow != 0) {
            ElectronWindowJni.get().destroy(mNativeElectronWindow, ElectronWindow.this);
            mNativeElectronWindow = 0;
        }
        
        if (mContentView != null && mContentView.getParent() != null) {
            ((ViewGroup) mContentView.getParent()).removeView(mContentView);
            mContentView = null;
        }
        
        mWebContents = null;
    }
    
    /**
     * Get the associated WebContents.
     */
    public WebContents getWebContents() {
        return mWebContents;
    }
    
    /**
     * Get the associated activity.
     */
    public ElectronActivity getActivity() {
        return mActivity;
    }
    
    @NativeMethods
    interface Natives {
        long init(ElectronWindow caller);
        void destroy(long nativeElectronWindow, ElectronWindow caller);
        WebContents createWebContents(long nativeElectronWindow, ElectronWindow caller);
        void onWebContentsCreated(long nativeElectronWindow, ElectronWindow caller, 
                WebContents webContents);
        void onDocumentLoadedInMainFrame(long nativeElectronWindow, ElectronWindow caller);
        void onNavigationFinished(long nativeElectronWindow, ElectronWindow caller);
        void onTitleChanged(long nativeElectronWindow, ElectronWindow caller, String title);
    }
}