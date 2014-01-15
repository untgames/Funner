package com.untgames.funner.push_notifications;

import android.app.Notification;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;

import java.util.Set;

import org.json.JSONStringer;

import com.amazon.device.messaging.ADM;
import com.amazon.device.messaging.ADMMessageHandlerBase;

/**
 * IntentService responsible for handling ADM messages.
 */
public class ADMService extends ADMMessageHandlerBase {
    private static final String TAG = "ADMService";

    /**
     * Class constructor.
     */
    public ADMService ()
    {
      super (ADMService.class.getName ());
    }
  
    /**
     * Class constructor, including the className argument.
     * 
     * @param className The name of the class.
     */
    public ADMService (final String className) 
    {
      super (className);
    }
    
    @Override
    protected void onRegistered (final String newRegistrationId)
    { 
      try
      {
        onRegisteredCallback (newRegistrationId);
      }
      catch (Throwable e)
      {
        Log.e (TAG, "Exception while processing registration: " + e);
      }
    }
     
    @Override
    protected void onUnregistered(final String registrationId)
    {
    }
     
    @Override
    protected void onRegistrationError(final String errorId)
    { 
      try
      {
        onErrorCallback (errorId);
      }
      catch (Throwable e)
      {
        Log.e (TAG, "Exception while processing error: " + e);
      }
    }

    @Override
    protected void onMessage(final Intent intent)
    { 
      try
      {
        //Send broadcast with this message
        Intent broadcastIntent = new Intent ("com.untgames.funner.push_notifications.PROCESS_ADM_MESSAGE");

        broadcastIntent.putExtras (intent);
        broadcastIntent.addCategory (getApplicationContext().getPackageName());

        sendBroadcast (broadcastIntent);

        //Send message to native code
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
      catch (Throwable e)
      {
        Log.e (TAG, "Exception while processing message: " + e);
      }
    }
    
    private native void onRegisteredCallback(String request);
    private native void onErrorCallback(String request);
    private native void onMessageCallback(String request);
}
