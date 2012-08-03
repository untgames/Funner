package com.untgames.funner.application;

import java.io.*;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;
import java.util.concurrent.locks.Condition;
import java.util.List;
import java.util.ArrayList;
import java.util.Collections;
import java.io.PrintWriter;
import java.io.StringWriter;
import android.app.Activity;
import android.view.View;
import android.util.*;

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

class UiAsyncResult
{
  boolean ready;
  
  public boolean isReady ()
  {
    synchronized (this)
    {
      return ready;
    }
  }
  
  protected void setReady ()
  {
    synchronized (this)
    {
      ready = true;
    }
  }
}

class UiAsyncBooleanResult extends UiAsyncResult
{
  volatile boolean value;
  
  public void setValue (boolean inValue)
  {
    synchronized (this)
    {
      this.value = inValue;
      setReady ();
    }
  }
  
  public boolean getValue ()
  {
    synchronized (this)
    {
      return value;
    }
  }
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
  
  static List    internalMessageQueue;
  static int     internalMessageProcessingDepth;
  static Object  internalMessageQueueMutex = new Object ();
  
  static List getInternalMessageQueue ()
  {
    try
    {
      synchronized (internalMessageQueueMutex)
      {
        return internalMessageQueue;
      }    
    }
    catch (Exception e)
    {
      return null;
    }
  }
  
  static class InternalMessage
  {
    public Activity activity;
    public Runnable runnable;
  }
  
  static List beginInternalMessageLoop () throws InterruptedException
  {
    synchronized (internalMessageQueueMutex)
    {
      if (internalMessageQueue == null)
      {
        internalMessageQueue           = Collections.synchronizedList (new ArrayList ());
        internalMessageProcessingDepth = 1;
      }                    
      else
      {
        internalMessageProcessingDepth++;
      }

      return internalMessageQueue;
    }              
  }
  
  static void endInternalMessageLoop () throws InterruptedException
  {
    synchronized (internalMessageQueueMutex)
    {      
      internalMessageProcessingDepth--;
        
      if (internalMessageProcessingDepth == 0)
      {
        while (!internalMessageQueue.isEmpty ())
        {
          InternalMessage message = (InternalMessage)internalMessageQueue.remove (0);
          
          message.activity.runOnUiThread (message.runnable);
        }
        
        internalMessageQueue = null;
      }
    }
  }
  
  public static Object run (Activity activity, UiRunnable runnable)
  {
    try
    {
      UiDispatch container = new UiDispatch (runnable);
      
      boolean needPostToUiThread = true;
      
      List queue = getInternalMessageQueue ();
      
      if (queue != null)
      {
        InternalMessage message = new InternalMessage ();
        
        message.activity = activity;
        message.runnable = container;
        
        queue.add (message);
      }
      else
      {
        activity.runOnUiThread (container);
      }

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
  
  public static void processMessagesInternally (UiAsyncResult asyncResult)
  {
    try
    {
      List queue = beginInternalMessageLoop ();
      
      if (queue == null)
        return;

      try
      {
        while (!asyncResult.isReady ())
        {
          InternalMessage message = null;
          
          if (!queue.isEmpty ())
          {
            try
            {                
              message = (InternalMessage)queue.remove (0);
            }
            catch (Exception e)
            {
            }
          }

          if (message == null)
          {
            Thread.currentThread ().sleep (1);
            continue;
          }

          message.runnable.run ();        
        }
      }
      finally
      {
        endInternalMessageLoop ();
      }
    }
    catch (Exception e)
    {
      StringWriter sw = new StringWriter();
      PrintWriter pw = new PrintWriter(sw);
            
      e.printStackTrace(pw);                        
        
      Log.e ("funner", sw.toString ());
    }
  }
}
