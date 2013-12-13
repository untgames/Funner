package com.untgames.funner.application;

import android.app.Activity;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.content.Context;
import android.content.ContextWrapper;
import android.content.Intent;
import android.content.SharedPreferences;
import android.graphics.Point;
import android.net.NetworkInfo;
import android.net.Uri;
import android.net.wifi.WifiInfo;
import android.net.wifi.WifiManager;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.os.Looper;
import android.os.Process;
import android.os.SystemClock;
import android.provider.Settings.Secure;
import android.util.*;
import android.view.Display;
import android.view.View;
import android.view.ViewGroup;
import android.view.Window;
import android.view.WindowManager;
import android.webkit.CookieManager;
import android.webkit.CookieSyncManager;
import android.widget.AbsoluteLayout;
import android.widget.FrameLayout;
import java.io.*;
import java.net.CookieHandler;
import java.security.MessageDigest;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Formatter;
import java.util.Iterator;
import java.util.List;
import java.util.Locale;
import java.util.UUID;
import org.apache.http.client.CookieStore;
import org.apache.http.impl.client.DefaultHttpClient;

/// Данный класс используется для запуска внешних shared-library
public class EngineActivity extends Activity
{
	///Типы посылаемых событий
	public enum EventType
	{
		ON_PAUSE, 
		ON_RESUME, 
		ON_STOP, 
		ON_START, 
		ON_DESTROY 
	}
	
	///Интерфейс слушателя событий
	public interface EngineActivityEventListener
	{
		void handleEngineActivityEvent (EventType eventType);
	}

	///Интерфейс слушателя результатов запуска других activity
	public interface EngineActivityResultListener
	{
		boolean handleEngineActivityResult (int requestCode, int resultCode, Intent data);
	}

	private static final String TAG = "funner";
			
  private static boolean isLoaded        = false;
  private ViewGroup      views           = null;
  private List           eventListeners  = new ArrayList ();
  private List           resultListeners = new ArrayList ();

///Загрузчик
  @Override
  public void onCreate(Bundle savedInstanceState)
  {        
    super.onCreate (savedInstanceState);    
    
    if (isLoaded)
      return;

    isLoaded = true;          
    
    startApplication ();    
  }        
  
  void startApplication ()
  {
      /// получение параметров запуска

    Bundle extras = getIntent ().getExtras ();
    
    if (extras == null)
    {
      Log.e (TAG, "No command line arguments found");
      System.exit (0);
      
      return;
    }

    String programName = extras.getString ("program");
    
    if (programName == null)
    {
      Log.e (TAG, "No 'program' command line argument found");
      System.exit (0);
      
      return;
    }
    
    String workDir = extras.getString ("workdir");
    
    if (workDir == null)
    {
      Log.e (TAG, "No 'workdir' command line argument found");
      System.exit (0);
      
      return;
    }    

    String programArgs     = extras.getString ("args");
    String librariesString = extras.getString ("libraries");
    
    ApplicationInfo appInfo = getApplicationInfo ();
    
    if (appInfo == null)
    {
      Log.e (TAG, "No ApplicationInfo attached");
      System.exit (0);      

      return;
    }

    String envVars = extras.getString ("envvars"); 
    
    if (envVars == null)
      envVars = "";
      
    ContextWrapper wrapper = new ContextWrapper (this);

    String sourceApk = appInfo.sourceDir;
    String dataDir   = appInfo.dataDir;
    String tmpDir    = wrapper.getCacheDir ().getPath ();
    
    envVars = envVars + " " + "APK_FULL_PATH='" + sourceApk + "'";
    envVars = envVars + " " + "HOME='" + dataDir + "'";
    envVars = envVars + " " + "TEMP='" + tmpDir + "'";
    envVars = envVars + " " + "ANDROID_DATA='" + getFilesDir ().getPath () + "'";
    
    String externalDataDir = null;

    if (Environment.getExternalStorageState ().equals (Environment.MEDIA_MOUNTED))
    	externalDataDir = getExternalFilesDir (null).getPath ();

    if (externalDataDir == null)
      externalDataDir = getFilesDir ().getPath ();

    envVars = envVars + " " + "ANDROID_EXTERNAL_DATA='" + externalDataDir + "'";

    try
    {
      if (librariesString != null)
      {
        String libraries [] = librariesString.split (" ");

        for (String library : libraries)
        {
          if (library != "")
            System.loadLibrary (library);
        }
      }              

      if (programName != "")
        System.load (programName);        

      setupHardwareConfiguration ();
      
      if (startApplication (programName, workDir, programArgs != null ? programArgs : "", envVars) == 0)
        System.exit (0);
    }    
    catch (Throwable e)
    {
      Log.e (TAG, e.getMessage());
      e.printStackTrace ();
      
      System.exit (0);
    }                   
  }

///Установка состояния скрин-сейвера
  public void setScreenSaverStateThreadSafe (final boolean state)
  {
    UiDispatch.run (this, new UiRunnable () {
      public Object run ()
      {
        Window mainWindow = getWindow ();
        
        if (state)
        {
          mainWindow.clearFlags (WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        }
        else
        {
          mainWindow.addFlags (WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        }
        
        return null;
      }
    });
  }  

///Установка параметров оборудования
  private void setupHardwareConfiguration ()
  {
    Display display = getWindowManager().getDefaultDisplay();
    DisplayMetrics metrics = new DisplayMetrics ();
    
    display.getMetrics (metrics);

    int width, height;
    
    if (Build.VERSION.SDK_INT >= 13)
    {
      Point size = new Point ();

      display.getSize (size);

      width  = size.x;
      height = size.y;
    }
    else
    {
      width  = display.getWidth ();
      height = display.getHeight ();
    }
    
    setScreenMode (width, height, (int)display.getRefreshRate (), (int)metrics.xdpi, (int)metrics.ydpi);
  }
  
///Приостановка приложения  
  @Override
  public void onPause ()
  {
    super.onPause ();
    
    notify (EventType.ON_PAUSE);
  }  
  
///Возобновление работы приложения
  @Override
  public void onResume ()
  {
    super.onResume ();
    
    notify (EventType.ON_RESUME);
  }    
    
///Остановка приложения  
  @Override
  public void onStop ()
  {
    onPauseCallback ();
    super.onStop ();
    
    notify (EventType.ON_STOP);
  }  
  
///Восстановление приложения
  @Override
  public void onStart ()
  {
    onResumeCallback ();
    super.onStart ();
    
    notify (EventType.ON_START);
  }    
    
///Завершение приложения
  @Override
  public void onDestroy ()
  {
    super.onDestroy ();
    
    notify (EventType.ON_DESTROY);
    
    android.os.Process.killProcess (android.os.Process.myPid ()); //kill our process, so singletons will die with activity. Activity is destroyed on device lock on Galaxy S3
  }        
    
///Нехватка памяти
  @Override
  public void onLowMemory ()  
  {
    onLowMemoryCallback ();
    super.onLowMemory ();
  }
  
///Игнорирование нажатия на кнопку back
  @Override
  public void onBackPressed ()
  {    
  }
  
///Обработка событий приложения
  protected void onApplicationNotification (String notification)
  {
  }
  
///Добавление окна
  void addView (View view)
  {
    boolean needSetContentView = views == null;
    
    if (needSetContentView)
    {
      views = new AbsoluteLayout (this);
    }
    
    views.addView (view, new AbsoluteLayout.LayoutParams (new ViewGroup.LayoutParams (FrameLayout.LayoutParams.WRAP_CONTENT, FrameLayout.LayoutParams.WRAP_CONTENT)));
    
    view.bringToFront ();
    view.requestFocus ();
    
    if (needSetContentView)    
      getWindow ().setContentView (views, new ViewGroup.LayoutParams (ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.MATCH_PARENT));
  }
  
/// Создание окна
  public EngineViewController createSurfaceViewController (String initString, final long windowRef)
  {
    final EngineActivity activity = this;            
    
    return (EngineViewController)UiDispatch.run (this, new UiRunnable () {
      public Object run ()
      {
        EngineViewController controller = new EngineSurfaceViewController (activity, windowRef);
        
        addView (controller.getView ());
        
        return controller;
      }
    });
  }
  
  public EngineViewController createWebViewController (String initString, final long windowRef)
  {
    final Activity activity = this;

    return (EngineViewController)UiDispatch.run (this, new UiRunnable () {
      public Object run ()
      {
        EngineViewController controller = new EngineWebViewController (activity, windowRef);        

        addView (controller.getView ());

//        controller.getView ().setVisibility (View.INVISIBLE);

        return controller;
      }
    });
  }
  
/// Получение UUID
  public String getUuid ()
  {
    SharedPreferences prefs = getSharedPreferences ("default.settings", 0);
    
    String uuid = prefs.getString ("UUID", null);
    
    if (uuid != null)
      return uuid;
    
    String buildString = null;
    
    String androidID = Secure.getString (getContentResolver(), Secure.ANDROID_ID);    
    
    if (androidID == null || androidID.equals ("9774d56d682e549c")) //magick android id - http://code.google.com/p/android/issues/detail?id=10603
    {
      buildString = UUID.randomUUID ().toString ();
    }
    else
    {
      buildString = androidID;
    }

    try
    {
      MessageDigest md = MessageDigest.getInstance ("SHA-1");
      
      uuid = byteArray2Hex (md.digest (buildString.getBytes ()));
      
      SharedPreferences.Editor prefsEditor = prefs.edit ();
      
      prefsEditor.putString ("UUID", uuid);
      
      prefsEditor.commit ();
      
      return uuid;
    }
    catch (Exception e)
    {
      return buildString;
    }
  }
  
  public String getSystemProperties ()
  {
    String result = "";
    
    result += " Board=" + Build.BOARD;
    result += " Brand=" + Build.BRAND;
    result += " CPU_ABI=" + Build.CPU_ABI;
    result += " Device=" + Build.DEVICE;
    result += " Display=" + Build.DISPLAY;
    result += " Host=" + Build.HOST;    
    result += " ID=" + Build.ID;
    result += " Manufacturer=" + Build.MANUFACTURER;
    result += " Model=" + Build.MODEL;
    result += " Product=" + Build.PRODUCT;
    result += " Tags=" + Build.TAGS;
    result += " Type=" + Build.TYPE;
    result += " User=" + Build.USER;
    result += " OSVersion=" + Build.VERSION.RELEASE;
    result += " SdkInt=" + Build.VERSION.SDK_INT;
    result += " Language=" + Locale.getDefault ().getLanguage () + "-" + Locale.getDefault ().getCountry ();
    result += " UUID=" + getUuid ();

    try
    {
    	PackageInfo packageInfo = getPackageManager().getPackageInfo(getPackageName(), 0);

    	result += " ApplicationVersion=" + packageInfo.versionName;
    }
    catch (Exception e)
    {
    	Log.e (TAG, "Exception while getting application version: " + e.getMessage ());
    }
    
    boolean hasWifi = false;
    
    try
    {
    	WifiManager wifiManager = (WifiManager)getSystemService (Context.WIFI_SERVICE);

    	if (wifiManager != null && wifiManager.getWifiState () == WifiManager.WIFI_STATE_ENABLED)
    	{
    		WifiInfo wifiInfo = wifiManager.getConnectionInfo ();
    		
    		if (wifiInfo != null)
    		{
    			hasWifi = wifiInfo.getSupplicantState () == android.net.wifi.SupplicantState.COMPLETED;
    		}
    	}
    }
    catch (Exception e)
    {
    	Log.e (TAG, "Exception while getting wifi state: " + e.getMessage ());
    	hasWifi = true;
    }
    
    if (!hasWifi)
    {
      result += " CellularOnlyInternet=1";
    }
    
    return result;
  }
  
  private static String byteArray2Hex (final byte[] hash) 
  {
    Formatter formatter = new Formatter ();
    
    for (byte b : hash) 
      formatter.format ("%02x", b);

    return formatter.toString ();
  }
  
/// Работа с cookies
  private void initCookieManager ()
  {
    // Edge case: an illegal state exception is thrown if an instance of
    // CookieSyncManager has not be created.  CookieSyncManager is normally
    // created by a WebKit view, but this might happen if you start the
    // app, restore saved state, and click logout before running a UI
    // dialog in a WebView -- in which case the app crashes
    @SuppressWarnings("unused")
    CookieSyncManager cookieSyncManager = CookieSyncManager.createInstance(this);    
  }
  
  public void setAcceptCookie (boolean accept)
  {
    initCookieManager ();
    
    CookieManager.getInstance ().setAcceptCookie (accept);
  }

  public boolean acceptCookie ()
  {
    initCookieManager ();
    
    return CookieManager.getInstance ().acceptCookie ();
  }

  public void deleteAllCookies ()
  {
    initCookieManager ();
    
    CookieManager manager = CookieManager.getInstance ();
    
    synchronized (manager) {
      manager.removeAllCookie ();
    }
  }

/// Открытие внешней ссылки
  public void openUrl (String url)
  {
    Intent intent = new Intent (Intent.ACTION_VIEW, Uri.parse (url));

    startActivity (intent);
  }

/// Работа с событиями
  public void addEventListener (EngineActivityEventListener listener)  
  {
    eventListeners.add (listener);
  }
  
  public void removeEventListener (EngineActivityEventListener listener)   
  {
    eventListeners.remove (listener);
  }
 
  private void notify (EventType eventType) 
  {
    Iterator i = eventListeners.iterator ();
    
    while (i.hasNext())  
    {
      ((EngineActivityEventListener) i.next()).handleEngineActivityEvent(eventType);
    }
  }
  
/// Работа с результатами запуска других активити
  public void addResultListener (EngineActivityResultListener listener)  
  {
    resultListeners.add (listener);
  }
  
  public void removeResultListener (EngineActivityResultListener listener)   
  {
    resultListeners.remove (listener);
  }
 
  protected void onActivityResult (int requestCode, int resultCode, Intent data)
  {
    Iterator i = resultListeners.iterator ();
    
    while (i.hasNext())  
    {
      if (((EngineActivityResultListener) i.next()).handleEngineActivityResult(requestCode, resultCode, data))  //result handled
      	return;
    }
    
    super.onActivityResult(requestCode, resultCode, data);
  }
  
/// Точка входа в native код
  public native int startApplication (String programName, String workDir, String programArgs, String envVars);  

/// Регистрация параметров дисплея
  public native void setScreenMode (int width, int height, int refresh_rate, int xdpi, int ydpi);  

/// Оповещение о возникновении событий
  public native void onPauseCallback ();
  public native void onResumeCallback ();
  public native void onLowMemoryCallback ();

/// Посылка сообщения в приложение
  public native void postNotification (String notification);
}
