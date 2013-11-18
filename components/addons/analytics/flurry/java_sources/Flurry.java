package com.untgames.funner.analytics;

import android.util.Log;

import com.untgames.funner.application.EngineActivity;

import com.flurry.android.FlurryAgent;

public class Flurry implements EngineActivity.EngineActivityEventListener
{
  private static final String TAG = "funner.Analytics.Flurry";
  
	private EngineActivity activity;
	private String         apiKey;
	private boolean        sessionStarted;

  public Flurry (EngineActivity inActivity, String inApiKey) 
  {
  	activity = inActivity;
  	apiKey   = inApiKey;

  	activity.addEventListener (this);
  }

	public void handleEngineActivityEvent (EngineActivity.EventType eventType)
	{
    switch (eventType)
    {
      case ON_START:
      	startSession ();
      	break;
      case ON_STOP:
      	endSession ();
      	break;
      default:
        break;
    }
	}
	
	private void startSession ()
	{
		if (sessionStarted)
			return;

  	FlurryAgent.onStartSession (activity, apiKey);
		
		sessionStarted = true;
	}
	
	private void endSession ()
	{
		if (!sessionStarted)
			return;
		
		FlurryAgent.onEndSession (activity);
		
		sessionStarted = false;
	}
}
