package com.untgames.android.funner_launcher;

import android.app.Activity;
import android.os.Bundle;
import android.util.*;
import java.io.*;

/// Данный класс используется для запуска внешних shared-library
public class SkeletonActivity extends Activity
{
///Загрузчик
  @Override
  public void onCreate(Bundle savedInstanceState)
  {
      /// получение параметров запуска
    
    Bundle extras = getIntent ().getExtras ();
    
    if (extras == null)
    {
      System.out.println ("No command line arguments found");
      System.exit (0);
      
      return;
    }
    
    String programName = extras.getString ("program");
    
    if (programName == null)
    {
      System.out.println ("No 'program' command line argument found");
      System.exit (0);
      
      return;
    }

    String programArgs = extras.getString ("args");
    
      //передача управления native коду
    
    try
    {
//      System.loadLibrary ("funner_launcher");
//      System.load ("/sdcard/funner/tmp/android/SYSTEMLIB.TEST_DLL/sources/test-dll/test_dll");
      System.load (programName);
//      System.loadLibrary (programName);      
      startApplication (programName, programArgs != null ? programArgs : "");
    }
    catch (Throwable e)
    {
      System.out.println (e.getMessage());
      e.printStackTrace ();
      
      System.exit (0);
    }       
    
    super.onCreate (savedInstanceState);
    
      //выход из приложения
    
    System.exit (0);
  }
  
/// Точка входа в native код
  public native void startApplication (String programName, String programArgs);
}
