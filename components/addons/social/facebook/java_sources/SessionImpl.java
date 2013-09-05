package com.untgames.funner.facebook_session;

import android.app.Activity;
import android.content.Context;
import android.util.Log;

import com.facebook.AppEventsLogger;

import com.untgames.funner.application.EngineActivity;

public class SessionImpl implements EngineActivity.EngineActivityEventListener
{
  private static final String TAG = "funner.facebook_session.SessionImpl";
  
  private EngineActivity activity;
	private String         applicationId;

	public SessionImpl (EngineActivity inActivity, String inApplicationId)
  {
		activity      = inActivity;
		applicationId = inApplicationId;
		
  	activity.addEventListener (this);
  }
  
	public void handleEngineActivityEvent (EngineActivity.EventType eventType)
	{
		if (eventType != EngineActivity.EventType.ON_RESUME)
			return;

		publishInstall ();
	}

  public void publishInstall ()
  {
  	activity.runOnUiThread (new Runnable () {
  		public void run ()
  		{
  			try
  			{
  				AppEventsLogger.activateApp (activity, applicationId);
  			}
  			catch (Throwable e)
  			{
  				Log.e (TAG, "Exception while publishing install: " + e.getMessage () + "\n stack: ");
  				e.printStackTrace ();
  			}
  		}
  	});
  }
}
