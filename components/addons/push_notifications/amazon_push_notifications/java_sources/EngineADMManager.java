package com.untgames.funner.push_notifications;

import android.content.Context;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageManager;
import android.util.Log;

import com.amazon.device.messaging.ADM;

//Класс для работы с сообщениями Amazon device messaging
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
  
	//Регистрация на получение сообщений
	public static void registerForADMMessages (Context context)
	{
    initializeADM (context);
		  
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

  public static void unregisterForADMMessages (Context context)
  {
    initializeADM (context);
    adm.startUnregister ();
  }

  private static native void onRegisteredCallback(String request);
  private static native void onErrorCallback(String error);
}
