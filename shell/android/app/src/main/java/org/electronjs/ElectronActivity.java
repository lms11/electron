package org.electronjs.app;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;

public class ElectronActivity extends Activity {
    private static final String TAG = "ElectronActivity";
    
    static {
        try {
            System.loadLibrary("electron");
            Log.i(TAG, "Successfully loaded electron native library");
        } catch (UnsatisfiedLinkError e) {
            Log.e(TAG, "Failed to load electron native library", e);
        }
    }
    
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        Log.i(TAG, "ElectronActivity onCreate");
        
        try {
            // Initialize Node.js and run Hello World
            runNodeHelloWorld();
        } catch (Exception e) {
            Log.e(TAG, "Failed to initialize Node.js", e);
        }
    }
    
    @Override
    protected void onDestroy() {
        super.onDestroy();
        Log.i(TAG, "ElectronActivity onDestroy");
    }
    
    // Native method declaration
    private native void runNodeHelloWorld();
}