package com.untgames.funner.application;

import android.app.Notification;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;

import java.util.Set;

import org.json.JSONStringer;

import com.google.android.gcm.GCMBaseIntentService;
import com.google.android.gcm.GCMRegistrar;

/**
 * IntentService responsible for handling GCM messages.
 */
public class GCMIntentService extends GCMBaseIntentService {
    private static final String TAG = "GCMIntentService";

    public GCMIntentService() {
      super();
    }

    /**
     * Gets the sender ids.
     */
    @Override
    protected String[] getSenderIds(Context context) {
    	String senderID = EngineGCMManager.getSenderId(context);

  		if (senderID == null)
   		{
   			Log.e (TAG, "'GCMSenderID' metadata not setted in application manifest");
   			return new String [0];
   		}

   		return new String [] { senderID };
    }
    
    @Override
    protected void onRegistered(final Context context, final String registrationId) {
      onRegisteredCallback (registrationId);
    }

    @Override
    protected void onUnregistered(Context context, String registrationId) {
    	//Do nothing
    }

    @Override
    protected void onMessage(Context context, final Intent intent) {
    	Bundle extras = intent.getExtras ();
    	
    	if (extras == null)
        onMessageCallback ("");
    	else
    	{
    		try
    		{
    			JSONStringer json = new JSONStringer ();

    			json.object ();

    			Set<String> extrasKeys = extras.keySet ();

    			for (String key : extrasKeys)
    			{
    				json.key (key);
    				json.value (extras.getString (key));
    			};

    			json.endObject ();

    			onMessageCallback (json.toString ());
    		}
    		catch (org.json.JSONException exception)
    		{
    			Log.e (TAG, "Exception while encoding message: " + exception);
    		}
    	}
    }

    @Override
    public void onError(Context context, final String errorId) {
      onErrorCallback (errorId);
    }

    @Override
    protected boolean onRecoverableError(Context context, String errorId) {
    	onError(context, errorId);
        return super.onRecoverableError(context, errorId);
    }

    public native void onRegisteredCallback(String request);
    public native void onErrorCallback(String request);
    public native void onMessageCallback(String request);
}
