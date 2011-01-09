package com.untgames.funner.application;

import java.io.*;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;
import java.util.concurrent.locks.Condition;
import android.app.Activity;
import android.view.View;

class UiCondition
{
  Lock      lock;
  Condition condition;
  boolean   state;
  
  public UiCondition ()
  {
    state     = false;
    lock      = new ReentrantLock ();
    condition = lock.newCondition ();
  }
  
  public void await () throws InterruptedException
  {
    lock.lock ();
    
    try
    {    
      while (!state)
        condition.await ();
    }
    finally
    {
      lock.unlock ();
    }
  }
  
  public void signal ()
  {
    lock.lock ();
    
    try
    {
      state = true;
        
      condition.signal ();
    }
    finally
    {
      lock.unlock ();
    }
  }
}

interface UiRunnable
{
  public Object run ();
}

class UiDispatch implements Runnable
{
  UiCondition condition;
  UiRunnable  runnable;
  Object      result;
  Exception   exception;
  
  UiDispatch (UiRunnable inRunnable)
  {
    runnable  = inRunnable;
    condition = new UiCondition ();
  }
  
  public void run ()
  {
    try
    {
      try
      {
        if (runnable != null)
          result = runnable.run ();              
      }
      catch (Exception e)
      {
        exception = e;
      }
    }
    finally
    {
      condition.signal ();
    }
  }
  
  public static Object run (Activity activity, UiRunnable runnable)
  {
    try
    {
      UiDispatch container = new UiDispatch (runnable);
      
      activity.runOnUiThread (container);
      
      container.condition.await ();
      
      if (container.exception != null)
        throw new RuntimeException (container.exception.getMessage (), container.exception);
      
      return container.result;
    }
    catch (InterruptedException e)
    {
      return null;
    }
  }
  
  public static Object run (View view, UiRunnable runnable)
  {
    return run ((Activity)view.getContext (), runnable);
  }
}
