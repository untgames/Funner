package com.untgames.funner.application;

import android.content.Context;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageManager;
import android.util.Log;

import com.google.android.gcm.GCMRegistrar;

//Класс для работы с сообщениями google cloud messaging
public class EngineGCMManager
{
  private static final String TAG = "EngineGCMManager";

  //Получение идентификатора отправителя
  public static String getSenderId(Context context) {
  		String packageName = context.getPackageName();
      int    resId       = context.getResources().getIdentifier("GCMSenderID", "string", packageName);
      String senderID    = context.getString(resId);
      
  		if (senderID == null)
  		{
  			Log.e (TAG, "'GCMSenderID' metadata not setted in application manifest");
  			return null;
  		}

  		return senderID;
  }
	
	//Регистрация на получение сообщений
	public static void registerForGCMMessages (Context context)
	{
		try
		{
  		GCMRegistrar.checkDevice(context);
	  	GCMRegistrar.checkManifest(context);
		}
		catch (Exception exception)
		{
			onErrorCallback ("Can't register for gcm, exception " + exception);
			return;
		}
		
		final String regId = GCMRegistrar.getRegistrationId(context);

		if (regId.equals("")) {
		  GCMRegistrar.register(context, getSenderId (context));
		} else {
			onRegisteredCallback(regId);
		}
  }
  
  public static native void onRegisteredCallback(String request);
  public static native void onErrorCallback(String error);
}
