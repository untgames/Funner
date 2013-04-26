package com.untgames.funner.store;

import android.app.Activity;
import android.content.Context;
import android.util.Log;

import com.untgames.funner.application.EngineActivity;

public class Store 
{
  private static final String TAG                   = "funner.Store";
  private static final int    PURCHASE_REQUEST_CODE = 343467;

	private static IabHelper      helper;
	private static EngineActivity activity;
	private static boolean        purchase_in_progress;
	
  public static void initialize (EngineActivity inActivity) 
  {
  	activity = inActivity;
  	
  	//TODO signup for activity onDestroy and dispose helper
  	
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
  
  public static void buyProduct (final Activity activity, final String sku, final int payloadValue)
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
  
  public static native void onInitializedCallback(boolean canBuyProducts);
}
