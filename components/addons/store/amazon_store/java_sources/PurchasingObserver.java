package com.untgames.funner.amazon_store;

import android.app.Activity;
import android.util.Log;

import com.amazon.inapp.purchasing.BasePurchasingObserver;
import com.amazon.inapp.purchasing.GetUserIdResponse;
import com.amazon.inapp.purchasing.ItemDataResponse;
import com.amazon.inapp.purchasing.PurchaseResponse;
import com.amazon.inapp.purchasing.PurchaseUpdatesResponse;

public class PurchasingObserver extends BasePurchasingObserver 
{
  private static final String LOG_TAG = "funner.AmazonPurchasingObserver";

  public PurchasingObserver (Activity iapActivity) 
  {
     super (iapActivity);
  }

  public void onSdkAvailable(final boolean isSandboxMode) 
  {
    Log.d (LOG_TAG, "SDK available, sandbox mode = " + isSandboxMode);
  }
  
  public void onGetUserIdResponse(final GetUserIdResponse response) {}
  public void onItemDataResponse(final ItemDataResponse response) {}
  public void onPurchaseResponse(final PurchaseResponse response) {}
  public void onPurchaseUpdatesResponse(final PurchaseUpdatesResponse response) {}
}