package com.untgames.funner.analytics;

import java.util.Map;

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

		FlurryAgent.init (activity, apiKey);
  	FlurryAgent.onStartSession (activity);
		
		sessionStarted = true;
	}
	
	private void endSession ()
	{
		if (!sessionStarted)
			return;
		
		FlurryAgent.onEndSession (activity);
		
		sessionStarted = false;
	}
	
	private static void logEvent (String eventId, Map< String, String > parameters,	boolean timed)
	{
		switch (FlurryAgent.logEvent (eventId, parameters, timed))
		{
		  case kFlurryEventFailed:
		  	Log.d (TAG, "Log event '" + eventId + "' failed");
		  	break;
		  case kFlurryEventRecorded:
		  	break;
		  case kFlurryEventUniqueCountExceeded:
		  	Log.d (TAG, "Log event '" + eventId + "' failed, unique count exceeded");
		  	break;
		  case kFlurryEventParamsCountExceeded:
		  	Log.d (TAG, "Log event '" + eventId + "' failed, params count exceeded");
		  	break;
		  case kFlurryEventLogCountExceeded:
		  	Log.d (TAG, "Log event '" + eventId + "' failed, log count exceeded");
		  	break;
		  case kFlurryEventLoggingDelayed:
		  	Log.d (TAG, "Log event '" + eventId + "' logging delayed");
		  	break;
		}
	}
}
