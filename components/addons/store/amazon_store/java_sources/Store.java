package com.untgames.funner.amazon_store;

import android.app.Activity;
import android.content.SharedPreferences;
import android.content.SharedPreferences.Editor;
import android.util.Log;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import java.util.Map;

import org.json.JSONObject;

import com.amazon.inapp.purchasing.BasePurchasingObserver;
import com.amazon.inapp.purchasing.GetUserIdResponse;
import com.amazon.inapp.purchasing.Item;
import com.amazon.inapp.purchasing.ItemDataResponse;
import com.amazon.inapp.purchasing.Offset;
import com.amazon.inapp.purchasing.PurchaseResponse;
import com.amazon.inapp.purchasing.PurchaseUpdatesResponse;
import com.amazon.inapp.purchasing.PurchasingManager;
import com.amazon.inapp.purchasing.Receipt;

public class Store
{
  private static final String LOG_TAG                          = "funner.AmazonStore";
  private static final String SAVED_PURCHASES_PREFERENCES_NAME = "com.untgames.funner.AmazonStore.SavedPurchases"; 
  
	private Activity               activity;
	private boolean                sdk_available;
	private String                 user_id;  
	private volatile boolean       purchase_in_progress;
	private volatile boolean       processing_stopped;
	private List<Runnable>         purchase_queue = Collections.synchronizedList (new ArrayList ());
	private String                 current_purchase_sku;  
	private java.util.Set<Receipt> purchased_skus = new java.util.HashSet<Receipt> ();
	private SharedPreferences      saved_purchases;

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
              onGetUserIdResponseImpl (response);
            }
            
            public void onItemDataResponse(final ItemDataResponse response) 
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
    sdk_available = true;

    PurchasingManager.initiateGetUserIdRequest (); //all applications are closed when user deregister device, so we can get user id only once per session
  }

  public void onGetUserIdResponseImpl (final GetUserIdResponse response) 
  {
    if (response.getUserIdRequestStatus () == GetUserIdResponse.GetUserIdRequestStatus.SUCCESSFUL) 
      user_id = response.getUserId ();
    
    onInitializedCallback (response.getUserIdRequestStatus () == GetUserIdResponse.GetUserIdRequestStatus.SUCCESSFUL);
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
    current_purchase_sku = null;

		activity.runOnUiThread (queue_processor);          		
	}
	
  public void buyProduct (final String sku)
  {
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
          
          PurchasingManager.initiatePurchaseUpdatesRequest (Offset.BEGINNING);
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
      if (response.getPurchaseUpdatesRequestStatus () == PurchaseUpdatesResponse.PurchaseUpdatesRequestStatus.FAILED)
      {
        onPurchaseFailedCallback (current_purchase_sku, "Purchase updates request failed");
        processPurchaseQueue ();      
        return;
      }
      
      purchased_skus.addAll (response.getReceipts ());
      
      if (response.isMore ())
      {
        PurchasingManager.initiatePurchaseUpdatesRequest (response.getOffset ());
        return;
      }
      
      //all purchases history received, check that item already purchased
      for (Receipt receipt : purchased_skus)
      {
        if (receipt.getSku ().equals (current_purchase_sku))
        {
          onPurchaseRestoredCallback (current_purchase_sku, user_id, receipt.getPurchaseToken ());
          processPurchaseQueue ();      
          return;
        }
      }
      
      SharedPreferences saved_purchases = getSavedPurchases ();
      
      Map<String, ?> all_purchase_data = saved_purchases.getAll ();
      
      for (Map.Entry<String, ?> purchase_data_entry : all_purchase_data.entrySet ())
      {
        PurchaseData purchase_data = PurchaseDataJSON.fromJSON ((String)purchase_data_entry.getValue ());
        
        if (purchase_data.sku.equals (current_purchase_sku))
        {
          onPurchaseRestoredCallback (current_purchase_sku, purchase_data.user_id, purchase_data.purchase_token);
          processPurchaseQueue ();      
          return;
        }
      }
      
      //this sku has not been purchased yet or consumable, launch purchase
      PurchasingManager.initiatePurchaseRequest (current_purchase_sku);
    }
    catch (Throwable e) 
    {
      onPurchaseFailedCallback (current_purchase_sku, e.getMessage ());
      processPurchaseQueue ();
    }
  }

  private SharedPreferences getSavedPurchases () 
  {
    if (saved_purchases != null)
      return saved_purchases;
    
    saved_purchases = activity.getSharedPreferences (SAVED_PURCHASES_PREFERENCES_NAME, Activity.MODE_PRIVATE);
    
    return saved_purchases;
  }
  
  private static class PurchaseData 
  {
    public String user_id;
    public String purchase_token;
    public String sku;

    public PurchaseData (String in_sku, String in_user_id, String in_purchase_token) 
    {
      user_id        = in_user_id;
      purchase_token = in_purchase_token;
      sku            = in_sku;
    }

    public String keyName () 
    {
      return purchase_token;
    }

  }

  private static class PurchaseDataJSON 
  {
    private static final String USER_ID = "userId";
    private static final String PURCHASE_TOKEN = "purchaseToken";
    private static final String SKU = "sku";

    public static String toJSON (PurchaseData data) throws org.json.JSONException
    {
      JSONObject obj = new JSONObject ();
      
      obj.put (USER_ID,        data.user_id);
      obj.put (PURCHASE_TOKEN, data.purchase_token);
      obj.put (SKU,            data.sku);

      return obj.toString ();
    }

    public static PurchaseData fromJSON (String json) throws org.json.JSONException
    {
      if (json == null)
        return null;
      
      JSONObject obj = new JSONObject(json);

      String user_id        = obj.getString (USER_ID);
      String purchase_token = obj.optString(PURCHASE_TOKEN);
      String sku            = obj.optString(SKU);

      return new PurchaseData (sku, user_id, purchase_token);
    }
  }
  
  private void saveConsumableReceipt (String sku, String userId, String purchaseToken) throws org.json.JSONException
  {
    PurchaseData purchase_data = new PurchaseData (sku, userId, purchaseToken);
    
    SharedPreferences saved_purchases = getSavedPurchases ();
    
    Editor editor = saved_purchases.edit ();
    editor.putString (purchase_data.keyName (), PurchaseDataJSON.toJSON (purchase_data));
    editor.apply ();
  }
  
  public void onPurchaseResponseImpl (final PurchaseResponse response) 
  {
    Receipt receipt = response.getReceipt ();
    
    try
    {
      if (response.getPurchaseRequestStatus () == PurchaseResponse.PurchaseRequestStatus.SUCCESSFUL)
      {
        //check that it is responce for currently purchasing item, store receipt anyway
        if ((!user_id.equals (response.getUserId ()) || !current_purchase_sku.equals (receipt.getSku ())) && receipt.getItemType () == Item.ItemType.CONSUMABLE)
        {
          saveConsumableReceipt (receipt.getSku (), response.getUserId (), receipt.getPurchaseToken ());
          return;
        }
      }
    }
    catch (Throwable e)
    {
      //ignore all errors
    }
    
    try
    {
      switch (response.getPurchaseRequestStatus ())
      {
        case FAILED:
        {
          onPurchaseFailedCallback (current_purchase_sku, "Purchase request failed");
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
          if (receipt != null)
          {
            saveConsumableReceipt (current_purchase_sku, user_id, receipt.getPurchaseToken ());
            
            onPurchaseSucceededCallback (current_purchase_sku, user_id, receipt.getPurchaseToken ());
            
            processPurchaseQueue ();      
          }
          else
          {
            onPurchaseFailedCallback (current_purchase_sku, "Purchase request failed");
            processPurchaseQueue ();      
          }
          
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

  public void finishTransaction (String purchase_token)
  {
  	try
  	{
      SharedPreferences saved_purchases = getSavedPurchases ();
      
      Editor editor = saved_purchases.edit ();
      editor.remove (purchase_token);
      editor.apply ();
  	  
      onTransactionFinishSucceededCallback (purchase_token);
    }
  	catch (Throwable e) 
  	{
      onTransactionFinishFailedCallback (purchase_token, e.getMessage ());
  	}
  }
  
  private native void onInitializedCallback (boolean canBuyProducts);
  private native void onPurchaseInitiatedCallback (String sku);
  private native void onPurchaseFailedCallback (String sku, String error);
  private native void onPurchaseSucceededCallback (String sku, String userId, String purchaseToken);
  private native void onPurchaseRestoredCallback (String sku, String userId, String purchaseToken);
  private native void onTransactionFinishFailedCallback (String purchaseToken, String error);
  private native void onTransactionFinishSucceededCallback (String purchaseToken);
}
