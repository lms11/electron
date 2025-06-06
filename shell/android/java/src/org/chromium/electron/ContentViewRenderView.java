// Copyright 2024 The Electron Authors. All rights reserved.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

package org.chromium.electron;

import android.content.Context;
import android.graphics.Color;
import android.graphics.PixelFormat;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.widget.FrameLayout;

import org.jni_zero.CalledByNative;
import org.jni_zero.JNINamespace;
import org.jni_zero.NativeMethods;
import org.chromium.electron.ContentViewRenderViewJni;

/**
 * A view that renders web content.
 * This is a simplified version for Electron's needs.
 */
@JNINamespace("electron")
public class ContentViewRenderView extends FrameLayout {
    private final SurfaceView mSurfaceView;
    private long mNativeContentViewRenderView;
    
    private ContentViewRenderView(Context context) {
        super(context);
        
        mSurfaceView = new SurfaceView(context);
        mSurfaceView.setZOrderMediaOverlay(true);
        mSurfaceView.getHolder().setFormat(PixelFormat.TRANSPARENT);
        mSurfaceView.getHolder().addCallback(new SurfaceHolder.Callback() {
            @Override
            public void surfaceCreated(SurfaceHolder holder) {
                if (mNativeContentViewRenderView != 0) {
                    ContentViewRenderViewJni.get().surfaceCreated(
                            mNativeContentViewRenderView, ContentViewRenderView.this);
                }
            }
            
            @Override
            public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
                if (mNativeContentViewRenderView != 0) {
                    ContentViewRenderViewJni.get().surfaceChanged(
                            mNativeContentViewRenderView, ContentViewRenderView.this,
                            format, width, height, holder.getSurface());
                }
            }
            
            @Override
            public void surfaceDestroyed(SurfaceHolder holder) {
                if (mNativeContentViewRenderView != 0) {
                    ContentViewRenderViewJni.get().surfaceDestroyed(
                            mNativeContentViewRenderView, ContentViewRenderView.this);
                }
            }
        });
        
        addView(mSurfaceView, new FrameLayout.LayoutParams(
                FrameLayout.LayoutParams.MATCH_PARENT,
                FrameLayout.LayoutParams.MATCH_PARENT));
        
        // Set background color.
        setBackgroundColor(Color.WHITE);
        
        // Initialize native side.
        mNativeContentViewRenderView = ContentViewRenderViewJni.get().init(this);
    }
    
    /**
     * Create a ContentViewRenderView.
     */
    public static ContentViewRenderView createContentViewRenderView(Context context) {
        return new ContentViewRenderView(context);
    }
    
    @Override
    protected void onSizeChanged(int w, int h, int oldw, int oldh) {
        super.onSizeChanged(w, h, oldw, oldh);
        if (mNativeContentViewRenderView != 0) {
            ContentViewRenderViewJni.get().onSizeChanged(
                    mNativeContentViewRenderView, ContentViewRenderView.this, w, h);
        }
    }
    
    /**
     * Destroy native resources.
     */
    public void destroy() {
        if (mNativeContentViewRenderView != 0) {
            ContentViewRenderViewJni.get().destroy(
                    mNativeContentViewRenderView, ContentViewRenderView.this);
            mNativeContentViewRenderView = 0;
        }
    }
    
    @CalledByNative
    @Override
    public void requestLayout() {
        post(new Runnable() {
            @Override
            public void run() {
                ContentViewRenderView.super.requestLayout();
            }
        });
    }
    
    @NativeMethods
    interface Natives {
        long init(ContentViewRenderView caller);
        void destroy(long nativeContentViewRenderView, ContentViewRenderView caller);
        void surfaceCreated(long nativeContentViewRenderView, ContentViewRenderView caller);
        void surfaceChanged(long nativeContentViewRenderView, ContentViewRenderView caller,
                int format, int width, int height, Surface surface);
        void surfaceDestroyed(long nativeContentViewRenderView, ContentViewRenderView caller);
        void onSizeChanged(long nativeContentViewRenderView, ContentViewRenderView caller,
                int width, int height);
    }
}