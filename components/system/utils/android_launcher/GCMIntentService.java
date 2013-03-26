package com.untgames.funner.application;

import android.app.Notification;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.content.Context;
import android.content.Intent;
import android.util.Log;

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

              //TODO call this later!!!!!!!!!
      GCMRegistrar.setRegisteredOnServer(context, true);
    }

    @Override
    protected void onUnregistered(Context context, String registrationId) {
     /*   if (GCMRegistrar.isRegisteredOnServer(context)) {
        } else {
            // This callback results from the call to unregister made on
            // ServerUtilities when the registration to the server failed.
        }*/
    }

    @Override
    protected void onMessage(Context context, final Intent intent) {
     	String message = new String ();
            	
    	Log.i (TAG, "Message received!!!!!!!! " + intent.toString ());
      	
    	//TODO fill message
      	
      onMessageCallback (message);
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
