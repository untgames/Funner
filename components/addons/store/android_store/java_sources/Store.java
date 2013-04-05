package com.untgames.funner.store;

import android.content.Context;
import android.util.Log;

public class Store 
{
  private static final String TAG = "funner.Store";

	private static IabHelper helper;
	
  public static void initialize (Context context) 
  {
    if (helper != null)
    	throw new IllegalStateException ("Already initialized");
    
    helper = new IabHelper (context);
    
    helper.startSetup (new IabHelper.OnIabSetupFinishedListener () {
      public void onIabSetupFinished(IabResult result)
      {
      	Log.d (TAG, "Setup finished, result: " + result);
      	
      	onInitializedCallback (result.isSuccess ());
      }
    });
  }
  
  public static native void onInitializedCallback(boolean canBuyProducts);
}
