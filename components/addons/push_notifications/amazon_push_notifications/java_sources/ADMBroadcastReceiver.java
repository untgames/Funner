package com.untgames.funner.push_notifications;

import android.content.Context;

import com.amazon.device.messaging.ADMMessageReceiver;

/**
 * Broadcast receiver for ADM messages
 */
public class ADMBroadcastReceiver extends ADMMessageReceiver {
  public ADMBroadcastReceiver ()
  {
    super(ADMService.class);
  }
  
  // Nothing else is required here; your broadcast receiver automatically
  // forwards intents to your service for processing.
}
