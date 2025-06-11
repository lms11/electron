// Copyright 2024 The Electron Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

package org.chromium.electron_shell;

import android.content.Context;
import android.util.AttributeSet;

import org.chromium.content_shell.Shell;
import org.chromium.content_shell.ShellManager;
import org.chromium.content_shell.R;

/**
 * Custom ShellManager for Electron that hides the navigation toolbar.
 */
public class ElectronShellManager extends ShellManager {
    
    /** Constructor for inflating via XML. */
    public ElectronShellManager(Context context, AttributeSet attrs) {
        super(context, attrs);
    }
    
    @Override
    public void launchShell(String url) {
        super.launchShell(url);
        
        // After launching the shell, hide the toolbar
        Shell activeShell = getActiveShell();
        if (activeShell != null) {
            android.view.View toolbar = activeShell.findViewById(R.id.toolbar);
            if (toolbar != null) {
                toolbar.setVisibility(android.view.View.GONE);
            }
        }
    }
}