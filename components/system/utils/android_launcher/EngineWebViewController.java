package com.untgames.funner.application;

import android.view.View;
import android.webkit.WebView;
import android.content.Context;
import android.graphics.Canvas;
import android.util.*;
import java.io.*;

public class EngineWebViewController extends EngineViewController
{  
  class EngineWebView extends WebView
  {
    private EngineWebViewController controller;
    
    public EngineWebView (Context context, EngineWebViewController controller)
    {    
      super (context);    
      
      this.controller = controller;
    }
    
    @Override
    protected void onLayout (boolean changed, int left, int top, int right, int bottom)
    {
      controller.onLayoutCallback (left, top, right, bottom);
    }    

    @Override  
    protected void onDisplayHint (int hint)
    {
      controller.onDisplayHintCallback (hint);
    }
    
    @Override
    protected void onDraw (Canvas c)
    {
      controller.onDrawCallback ();
    }    
  }
  
  WebView view;

  public EngineWebViewController (Context context, long window_ref)
  {
    view = new EngineWebView (context, this);
    
    initialize (context, view, window_ref);
  }  
  
  public void loadUrlThreadSafe (String in_url)
  {
    final String url = in_url;
    
    UiDispatch.run (view, new UiRunnable () {
      public Object run ()
      {
        view.loadUrl (url); 
        
        return null;
      }
    });
  }
  
  public void reloadThreadSafe ()
  {
    UiDispatch.run (view, new UiRunnable () {
      public Object run ()
      {
        view.reload (); 
        
        return null;
      }
    });
  }  
}
