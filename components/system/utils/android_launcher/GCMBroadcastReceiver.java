package com.untgames.funner.application;

import android.content.Context;

/**
 * Broadcast receiver for GCM messages
 */
public class GCMBroadcastReceiver extends com.google.android.gcm.GCMBroadcastReceiver {
    /**
     * Get intent service class name
     */
    @Override
    protected String getGCMIntentServiceClassName (Context context)
    {
    	return "com.untgames.funner.application.GCMIntentService";
    }
}
