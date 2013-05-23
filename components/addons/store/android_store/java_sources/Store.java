package com.untgames.funner.store;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.util.Log;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

import com.untgames.funner.application.EngineActivity;

public class Store implements EngineActivity.EngineActivityEventListener, EngineActivity.EngineActivityResultListener
{
  private static final String TAG                   = "funner.Store";
  private static final int    PURCHASE_REQUEST_CODE = 343467;
  
	private IabHelper        helper;
	private EngineActivity   activity;
	private volatile boolean purchase_in_progress;
	private List<Runnable>   purchase_queue = Collections.synchronizedList (new ArrayList ());
	
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

  public void buyProduct (final Activity activity, final String sku)
  {
  	final Runnable queue_processor = new Runnable () {
  		public void run ()
  		{
  			if (purchase_queue.isEmpty ())
  				return;
  			
    		if (purchase_in_progress)
  	  		return;
  			
    		purchase_in_progress = true;
    		
    		Runnable next_request = purchase_queue.remove (0);

    		next_request.run ();
  		}  		
  	};
  	
  	Runnable request = new Runnable () {
  		public void run ()
  		{
  			try
  			{
    	  	onPurchaseInitiatedCallback (sku);
  			}
  			catch (Exception e)
  			{
  				//ignore all exceptions
  			}
  	  	
  			helper.launchPurchaseFlow (activity, sku, PURCHASE_REQUEST_CODE, new IabHelper.OnIabPurchaseFinishedListener ()
  			{
          public void onIabPurchaseFinished(IabResult result, Purchase info)
          {
          	try
          	{
          		Log.d (TAG, "Purchase '" + info + "' finished, result: " + result);

          		if (result.isFailure ()) 
          			onPurchaseFailedCallback (sku, result.getMessage ());
          		else
          		{
          			onPurchaseSucceededCallback (sku);

          			//TODO restored callback
          		}
          	}
          	finally
          	{
          		purchase_in_progress = false;

          		activity.runOnUiThread (queue_processor);          		
          	}
          }
  			});
  		}
  	};

		purchase_queue.add (request);

  	activity.runOnUiThread (queue_processor);
  }
  
  public native void onInitializedCallback(boolean canBuyProducts);
  public native void onPurchaseInitiatedCallback(String sku);
  public native void onPurchaseFailedCallback(String sku, String error);
  public native void onPurchaseSucceededCallback(String sku);
  public native void onPurchaseRestoredCallback(String sku);
}
