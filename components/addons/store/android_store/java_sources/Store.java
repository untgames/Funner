package com.untgames.funner.store;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.util.Log;

import com.untgames.funner.application.EngineActivity;

public class Store implements EngineActivity.EngineActivityEventListener, EngineActivity.EngineActivityResultListener
{
  private static final String TAG                   = "funner.Store";
  private static final int    PURCHASE_REQUEST_CODE = 343467;

	private IabHelper      helper;
	private EngineActivity activity;
	private boolean        purchase_in_progress;
	
  public void initialize (EngineActivity inActivity) 
  {
  	if (activity != null)
  	{
  		activity.removeEventListener (this);
  		activity.removeResultListener (this);
  	}
  	
  	activity = inActivity;
  	
  	activity.addEventListener (this);
  	activity.addResultListener (this);
  	
    if (helper != null)
    	throw new IllegalStateException ("Already initialized");
    
    helper = new IabHelper (activity);
    
    helper.startSetup (new IabHelper.OnIabSetupFinishedListener () {
      public void onIabSetupFinished(IabResult result)
      {
      	Log.d (TAG, "Setup finished, result: " + result);
      	
      	onInitializedCallback (result.isSuccess ());
      }
    });
  }

	public void handleEngineActivityEvent (EngineActivity.EventType eventType)
	{
		if (eventType != EngineActivity.EventType.ON_DESTROY)
			return;

    Log.d (TAG, "Destroying helper.");
    
    if (helper != null)
    {
      helper.dispose ();
      helper = null;
    }
	}
	
	public boolean handleEngineActivityResult (int requestCode, int resultCode, Intent data)
	{
    return helper.handleActivityResult (requestCode, resultCode, data);
	}

  public void buyProduct (final Activity activity, final String sku, final int payloadValue)
  {
  	//TODO make purchase queue
  	
  	if (purchase_in_progress)
  		throw new IllegalStateException ("Another purchase in progress");
  	
  	purchase_in_progress = true;
  	
  	activity.runOnUiThread (new Runnable () {
  		public void run ()
  		{
  			helper.launchPurchaseFlow (activity, sku, PURCHASE_REQUEST_CODE, new IabHelper.OnIabPurchaseFinishedListener ()
  			{
          public void onIabPurchaseFinished(IabResult result, Purchase info)
          {
          	Log.d (TAG, "Purchase '" + info + "' finished, result: " + result);
          	purchase_in_progress = false;
          }
  			}, Integer.toString (payloadValue));
  		}
  	});
  }
  
  public native void onInitializedCallback(boolean canBuyProducts);
}
