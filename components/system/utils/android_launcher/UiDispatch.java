package com.untgames.funner.application;

///помещать сообщения всегда во внутренню очередь; во внешшнюю пппомещать сообщение о необходимости внутренней очереди

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
  
  static List internalMessageQueue = Collections.synchronizedList (new ArrayList ());
    
  static class InternalMessage
  {
    public Activity activity;
    public Runnable runnable;
  }

  static boolean dispatchMessage ()
  {        
    if (internalMessageQueue.isEmpty ())
      return false;

    InternalMessage message = (InternalMessage)internalMessageQueue.remove (0);

    if (message == null)
      return false;

    message.runnable.run ();

    return true;
  }

  static class MessageDispatcher implements Runnable
  {
    public void run ()
    {
      dispatchMessage ();
    }
  }    
  
  public static Object run (Activity activity, UiRunnable runnable)
  {
    try
    {
      UiDispatch container = new UiDispatch (runnable);
      
      InternalMessage message = new InternalMessage ();
        
      message.activity = activity;
      message.runnable = container;
        
      internalMessageQueue.add (message);

      activity.runOnUiThread (new MessageDispatcher ());

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
    while (!asyncResult.isReady ())
    {
      try
      {
        if (!dispatchMessage ())
          Thread.currentThread ().sleep (1);
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
}
