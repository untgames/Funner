package com.untgames.funner.amazon_store;

import android.app.Activity;
import android.util.Log;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

import com.amazon.inapp.purchasing.BasePurchasingObserver;
import com.amazon.inapp.purchasing.GetUserIdResponse;
import com.amazon.inapp.purchasing.ItemDataResponse;
import com.amazon.inapp.purchasing.PurchaseResponse;
import com.amazon.inapp.purchasing.PurchaseUpdatesResponse;
import com.amazon.inapp.purchasing.PurchasingManager;

public class Store
{
  private static final String LOG_TAG = "funner.AmazonStore";
  
	private Activity         activity;
	private volatile boolean purchase_in_progress;
	private volatile boolean processing_stopped;
	private List<Runnable>   purchase_queue = Collections.synchronizedList (new ArrayList ());

  public Store (Activity inActivity)
  {
    activity = inActivity;

    activity.runOnUiThread (new Runnable () {
      public void run ()
      {
        try
        {
          PurchasingManager.registerObserver (new BasePurchasingObserver (activity)
          {
            public void onSdkAvailable(final boolean isSandboxMode) 
            {
              onSdkAvailableImpl ();
            }
            
            public void onGetUserIdResponse(final GetUserIdResponse response) 
            {
              Log.d (LOG_TAG, "USER ID RESPONSE: " + response);
            }
            
            public void onItemDataResponse(final ItemDataResponse response) 
            {
              Log.d (LOG_TAG, "ITEM DATA RESPONSE: " + response);
            }
            
            public void onPurchaseResponse(final PurchaseResponse response) 
            {
              Log.d (LOG_TAG, "PURCHASE RESPONSE: " + response);
            }
            
            public void onPurchaseUpdatesResponse(final PurchaseUpdatesResponse response) 
            {
              Log.d (LOG_TAG, "PURCHASE UPDATES RESPONSE: " + response);
            }
          });
        }
        catch (Throwable e)
        {
          Log.d (LOG_TAG, "Exception while creating store object: " + e.getMessage ());
        }
      }
    });
  }

  public void onSdkAvailableImpl ()
  {
    onInitializedCallback (true);
  }
  
	private final Runnable queue_processor = new Runnable () {
		public void run ()
		{
			if (processing_stopped)
				return;
			
			if (purchase_queue.isEmpty ())
				return;
			
  		if (purchase_in_progress)
	  		return;
			
  		purchase_in_progress = true;
  		
  		Runnable next_request = purchase_queue.remove (0);

  		next_request.run ();
		}  		
	};

  public void stopProcessing ()
  {
  	processing_stopped = false;
  }
  
	private void processPurchaseQueue ()
	{
		purchase_in_progress = false;

		activity.runOnUiThread (queue_processor);          		
	}
	
  public void buyProduct (final String sku)
  {
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
  	  	
  			
        String requestId = PurchasingManager.initiatePurchaseRequest (sku); //DEBUG
  			
  			
  			
  			  //check if this item already bought
/*  			helper.queryInventoryAsync (false, new IabHelper.QueryInventoryFinishedListener ()
  			{
          public void onQueryInventoryFinished(IabResult result, Inventory inv)
          {
          	try
          	{
          		Log.d(TAG, "Query inventory '" + inv + "' finished, result: " + result);

          		if (result.isFailure())
          		{
          			onPurchaseFailedCallback (sku, result.getMessage ());
            		processPurchaseQueue ();
          		}
          		else
          		{
                Purchase info = inv.getPurchase (sku);
                
                if (info != null)
                {
            			onPurchaseRestoredCallback (sku, info, info.getOriginalJson (), info.getSignature ());
              		processPurchaseQueue ();
                	return;
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
                  			onPurchaseSucceededCallback (sku, info, info.getOriginalJson (), info.getSignature ());
                  	}
                  	finally
                  	{
                  		processPurchaseQueue ();
                    }
                  }
          			});
          		}
          	}
          	catch (Throwable e) 
          	{
          		processPurchaseQueue ();
          	}
          }
  			});*/
  		}
  	};

		purchase_queue.add (request);

  	activity.runOnUiThread (queue_processor);
  }

  public void consumeProduct ()
  {
/*    (new Thread (new Runnable () {
      public void run () {
      	try
      	{
          helper.consume(purchase);
          onConsumeSucceededCallback (purchase);
        }
        catch (IabException e) {
          onConsumeFailedCallback (purchase, e.getResult ().getMessage ());
        }
      	catch (Throwable e) {
          onConsumeFailedCallback (purchase, e.getMessage ());
      	}
      }
    })).start ();*/
  }
  
  private native void onInitializedCallback(boolean canBuyProducts);
  private native void onPurchaseInitiatedCallback(String sku);
  private native void onPurchaseFailedCallback(String sku, String error);
  private native void onPurchaseSucceededCallback(String sku, String receipt, String signature);
  private native void onPurchaseRestoredCallback(String sku, String receipt, String signature);
  private native void onConsumeFailedCallback(String error);
  private native void onConsumeSucceededCallback();
}
