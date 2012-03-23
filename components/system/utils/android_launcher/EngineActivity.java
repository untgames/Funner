package com.untgames.funner.application;

import android.app.Activity;
import android.view.View;
import android.view.ViewGroup;
import android.os.Bundle;
import android.os.Looper;
import android.content.ContextWrapper;
import android.content.pm.ApplicationInfo;
import android.util.*;
import java.io.*;

/// Данный класс используется для запуска внешних shared-library
public class EngineActivity extends Activity
{
///Загрузчик
  @Override
  public void onCreate(Bundle savedInstanceState)
  {
    super.onCreate (savedInstanceState);
    
    Log.i ("funner", "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!onCreate");
    
      /// получение параметров запуска

    Bundle extras = getIntent ().getExtras ();
    
    if (extras == null)
    {
      Log.e ("funner", "No command line arguments found");
      System.exit (0);
      
      return;
    }

    String programName = extras.getString ("program");
    
    if (programName == null)
    {
      Log.e ("funner", "No 'program' command line argument found");
      System.exit (0);
      
      return;
    }
    
    String workDir = extras.getString ("workdir");
    
    if (workDir == null)
    {
      Log.e ("funner", "No 'workdir' command line argument found");
      System.exit (0);
      
      return;
    }    

    String programArgs     = extras.getString ("args");
    String librariesString = extras.getString ("libraries");
    
    ApplicationInfo appInfo = getApplicationInfo ();
    
    if (appInfo == null)
    {
      Log.e ("funner", "No ApplicationInfo attached");
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

    try
    {
      if (android.os.Build.VERSION.SDK_INT < android.os.Build.VERSION_CODES.GINGERBREAD)
        System.loadLibrary ("android");
        
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

      if (startApplication (programName, workDir, programArgs != null ? programArgs : "", envVars) == 0)
        System.exit (0);
    }    
    catch (Throwable e)
    {
      Log.e ("funner", e.getMessage());
      e.printStackTrace ();
      
      System.exit (0);
    }           
  }  
  
///Приостановка приложения  
  @Override
  public void onPause ()
  {
    onPauseCallback ();
    super.onPause ();
  }  
  
///Восстановление приложения
  @Override
  public void onResume ()
  {
    onResumeCallback ();
    super.onResume ();
  }    
  
///Завершение приложения
  @Override
  public void onDestroy ()
  {
    super.onDestroy ();
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
  
/// Создание окна
  public View createEngineView (String initString, final long windowRef)
  {
    final Activity activity = this;    
    
    return (View)UiDispatch.run (this, new UiRunnable () {
      public Object run ()
      {
        View view = new EngineView (activity, windowRef);
        
        getWindow ().addContentView (view, new ViewGroup.LayoutParams (ViewGroup.LayoutParams.WRAP_CONTENT, ViewGroup.LayoutParams.WRAP_CONTENT));        
        
        return view;
      }
    });
  }

/// Точка входа в native код
  public native int startApplication (String programName, String workDir, String programArgs, String envVars);

/// Оповещение о возникновении событий
  public native void onPauseCallback ();
  public native void onResumeCallback ();
  public native void onLowMemoryCallback ();
}
