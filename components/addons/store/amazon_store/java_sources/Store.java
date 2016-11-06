package com.untgames.funner.amazon_store;

import android.app.Activity;
import android.util.Log;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

import org.json.JSONObject;

import com.amazon.device.iap.PurchasingListener;
import com.amazon.device.iap.PurchasingService;
import com.amazon.device.iap.model.FulfillmentResult;
import com.amazon.device.iap.model.ProductDataResponse;
import com.amazon.device.iap.model.ProductType;
import com.amazon.device.iap.model.PurchaseResponse;
import com.amazon.device.iap.model.PurchaseUpdatesResponse;
import com.amazon.device.iap.model.Receipt;
import com.amazon.device.iap.model.UserDataResponse;

import com.untgames.funner.application.EngineActivity;

public class Store implements EngineActivity.EngineActivityEventListener
{
  private static final String LOG_TAG = "funner.AmazonStore";
  
	private EngineActivity         activity;
  private volatile boolean       initialized;
  private String                 previous_user_id;  
	private String                 user_id;  
	private boolean                needs_update_user_id;
	private volatile boolean       purchase_in_progress;
	private volatile boolean       processing_stopped;
	private List<Runnable>         purchase_queue = Collections.synchronizedList (new ArrayList ());
	private String                 current_purchase_sku;  
	private java.util.Set<Receipt> purchased_skus = new java.util.HashSet<Receipt> ();

  public Store (EngineActivity inActivity)
  {
    activity = inActivity;
    
    activity.addEventListener (this);

    activity.runOnUiThread (new Runnable () {
      public void run ()
      {
        try
        {
          PurchasingService.registerListener (activity, new PurchasingListener ()
          {
            public void onUserDataResponse(final UserDataResponse response) 
            {
              onUserDataResponseImpl (response);
            }
            
            public void onProductDataResponse(final ProductDataResponse productDataResponse)
            {
              //TODO
            }
            
            public void onPurchaseResponse(final PurchaseResponse response) 
            {
              onPurchaseResponseImpl (response);
            }
            
            public void onPurchaseUpdatesResponse(final PurchaseUpdatesResponse response) 
            {
              onPurchaseUpdatesResponseImpl (response);
            }
          });

          PurchasingService.getUserData ();
        }
        catch (Throwable e)
        {
          Log.d (LOG_TAG, "Exception while creating store object: " + e.getMessage ());
        }
      }
    });
  }

  private synchronized String getUserId ()
  {
    return user_id;
  }
  
  private synchronized void setUserId (String new_user_id)
  {
    user_id = new_user_id;
  }
  
  public void handleEngineActivityEvent (EngineActivity.EventType eventType)
  {
    switch (eventType)
    {
      case ON_STOP:
      {
        if (!needs_update_user_id)
        {
          needs_update_user_id = true;
        
          setUserId (null);
        
          break;
        }
      }
      case ON_START:
      {
        if (needs_update_user_id)
        {
          needs_update_user_id = false;
          
          PurchasingService.getUserData ();
        }
        
        break;
      }
      default:
        break;
    }
  }

  public void onUserDataResponseImpl (final UserDataResponse response) 
  {
    String new_user_id = null; 
    
    if (response.getRequestStatus () == UserDataResponse.RequestStatus.SUCCESSFUL)
    {
      new_user_id = response.getUserData ().getUserId ();
      setUserId (new_user_id);
    }
    
    if (previous_user_id != null && !previous_user_id.equals (new_user_id))  //user changed, clear queue so we don't process purchases initiated by another user
      purchase_queue.clear ();
    
    previous_user_id = new_user_id;

    if (!initialized)
    {
      initialized = true;
      onInitializedCallback (response.getRequestStatus () == UserDataResponse.RequestStatus.SUCCESSFUL);
    }
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

  		if (getUserId () == null)
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
    current_purchase_sku = null;

		activity.runOnUiThread (queue_processor);          		
	}
	
  public void buyProduct (final String sku)
  {
    if (user_id == null)
    {
      onPurchaseFailedCallback (sku, "User id not obtained");
      return;
    }
    
  	Runnable request = new Runnable () {
  		public void run ()
  		{
  		  current_purchase_sku = sku;
  		  
  			try
  			{
    	  	onPurchaseInitiatedCallback (sku);
  			}
  			catch (Exception e)
  			{
  				//ignore all exceptions
  			}
  			
  			  //update list of already purchased items
        try
        {
          purchased_skus.clear ();
          
          PurchasingService.getPurchaseUpdates (true);
        }
        catch (Throwable e) 
        {
          processPurchaseQueue ();
        }
  		}
  	};

		purchase_queue.add (request);

  	activity.runOnUiThread (queue_processor);
  }

  public void onPurchaseUpdatesResponseImpl (final PurchaseUpdatesResponse response) 
  {
    try
    {
      if (response.getRequestStatus () != PurchaseUpdatesResponse.RequestStatus.SUCCESSFUL)
      {
        onPurchaseFailedCallback (current_purchase_sku, "Purchase updates request failed");
        processPurchaseQueue ();      
        return;
      }
      
      purchased_skus.addAll (response.getReceipts ());
      
      if (response.hasMore ())
      {
        PurchasingService.getPurchaseUpdates (false);
        return;
      }
      
      String current_user_id = getUserId ();
      
      //all purchases history received, check that item already purchased
      for (Receipt receipt : purchased_skus)
      {
        if (receipt.getSku ().equals (current_purchase_sku) && !receipt.isCanceled ())
        {
          onPurchaseRestoredCallback (current_purchase_sku, current_user_id, receipt.getReceiptId ());
          processPurchaseQueue ();      
          return;
        }
      }
      
      //this sku has not been purchased yet or consumable, launch purchase
      PurchasingService.purchase (current_purchase_sku);
    }
    catch (Throwable e) 
    {
      onPurchaseFailedCallback (current_purchase_sku, e.getMessage ());
      processPurchaseQueue ();
    }
  }
  
  public void onPurchaseResponseImpl (final PurchaseResponse response) 
  {
    String  current_user_id = getUserId ();
    Receipt receipt         = response.getReceipt ();
    
    try
    {
      if (response.getRequestStatus () == PurchaseResponse.RequestStatus.SUCCESSFUL)
      {
        //check that it is responce for currently purchasing item
        if ((current_user_id == null || (!current_user_id.equals (response.getUserData ().getUserId ()) || !current_purchase_sku.equals (receipt.getSku ()))) && receipt.getProductType () == ProductType.CONSUMABLE)
          return;
      }
    }
    catch (Throwable e)
    {
      //ignore all errors
    }
    
    try
    {
      switch (response.getRequestStatus ())
      {
        case ALREADY_PURCHASED:
        case SUCCESSFUL:
        {
          if (receipt != null)
          {
            onPurchaseSucceededCallback (current_purchase_sku, current_user_id, receipt.getReceiptId ());
            
            processPurchaseQueue ();      
          }
          else
          {
            onPurchaseFailedCallback (current_purchase_sku, "Purchase request failed");
            processPurchaseQueue ();      
          }
          
          return;
        }
        case NOT_SUPPORTED:
        {
          onPurchaseFailedCallback (current_purchase_sku, "Purchase request failed, not supported");
          processPurchaseQueue ();      
          return;
        }
        case INVALID_SKU:
        {
          onPurchaseFailedCallback (current_purchase_sku, "Purchase request failed, invalid sku");
          processPurchaseQueue ();      
          return;
        }
        default:
        {
          onPurchaseFailedCallback (current_purchase_sku, "Purchase request failed");
          processPurchaseQueue ();      
          return;
        }
      }
    }
    catch (Throwable e) 
    {
      onPurchaseFailedCallback (current_purchase_sku, e.getMessage ());
      processPurchaseQueue ();
    }
  }

  public void finishTransaction (String receipt_id)
  {
  	try
  	{
  	  //TODO add support for unavailable
  	  
  	  PurchasingService.notifyFulfillment(receipt_id, FulfillmentResult.FULFILLED);

      onTransactionFinishSucceededCallback (receipt_id);
    }
  	catch (Throwable e) 
  	{
      onTransactionFinishFailedCallback (receipt_id, e.getMessage ());
  	}
  }
  
  private native void onInitializedCallback (boolean canBuyProducts);
  private native void onPurchaseInitiatedCallback (String sku);
  private native void onPurchaseFailedCallback (String sku, String error);
  private native void onPurchaseSucceededCallback (String sku, String userId, String receiptId);
  private native void onPurchaseRestoredCallback (String sku, String userId, String receiptId);
  private native void onTransactionFinishFailedCallback (String purchaseToken, String error);
  private native void onTransactionFinishSucceededCallback (String receiptId);
}
