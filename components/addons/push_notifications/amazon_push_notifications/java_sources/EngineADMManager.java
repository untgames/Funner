package com.untgames.funner.push_notifications;

import android.app.Activity;
import android.content.Context;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageManager;
import android.util.Log;

import com.amazon.device.messaging.ADM;

//Class for work with Amazon device messaging
public class EngineADMManager
{
  private static final String TAG = "EngineADMManager";

  private static ADM adm; 
  
  private static void initializeADM (Context context)
  {
    if (adm != null)
      return;
    
    adm = new ADM (context);
  }
  
  //Registering for messages receiving
	public static void registerForADMMessages (final Context context) throws Throwable
	{
	  try
	  {
      initializeADM (context);

//      com.amazon.device.messaging.development.ADMManifest.checkManifestAuthoredProperly (activity); //DO NOT USE IN PRODUCTION
    
      final String regId = adm.getRegistrationId ();

      if (regId == null || regId.equals ("")) 
      {
        adm.startRegister ();
      } 
      else 
      {
        onRegisteredCallback (regId);
      }
    }
    catch (Throwable e)
    {
      Log.e (TAG, "Can't register for adm, exception " + e);
      onErrorCallback ("Can't register for adm, exception " + e);
      throw e;
    }
  }

  public static void unregisterForADMMessages (final Context context)
  {
    try
    {
      initializeADM (context);
      adm.startUnregister ();
    }
    catch (Throwable e)
    {
      Log.e (TAG, "Can't unregister from ADM, exception " + e);
    }
  }
  
  public static boolean isAPIAvailable ()
  {
    try
    {
      Class.forName ("com.amazon.device.messaging.ADM");
    }
    catch (ClassNotFoundException e)
    {
      return false;
    }

    return true;
  }

  private static native void onRegisteredCallback(String request);
  private static native void onErrorCallback(String error);
}
