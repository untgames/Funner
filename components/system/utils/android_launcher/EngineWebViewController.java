package com.untgames.funner.application;

import android.view.View;
import android.webkit.WebView;
import android.webkit.WebViewClient;
import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Bitmap;
import android.util.*;
import java.io.*;

public class EngineWebViewController extends EngineViewController
{  
  class EngineWebViewClient extends WebViewClient
  {
    private EngineWebViewController controller;
    private boolean is_loading;
    
    EngineWebViewClient (EngineWebViewController controller)
    {
      this.controller = controller;
    }
    
    public boolean isLoading () { return is_loading; }
    
    @Override
    public boolean shouldOverrideUrlLoading(WebView view, String url)
    {
      Log.e ("funner", url);        
      return false;
    }    
    
    @Override
    public void onPageStarted (WebView view, String url, Bitmap favicon)
    {
      is_loading = true;
    }

    @Override
    public void onPageFinished (WebView view, String url) 
    {

    }
    
    @Override
    public void onReceivedError (WebView view, int errorCode, String description, String failingUrl)
    {
    }
  }

  class EngineWebView extends WebView
  {
    private EngineWebViewController controller;    
    
    public EngineWebView (Context context, EngineWebViewController controller)
    {    
      super (context);    
      
      this.controller = controller;

      getSettings ().setJavaScriptEnabled (true);

      setWebViewClient (new EngineWebViewClient (controller));      
      
      requestFocus (View.FOCUS_DOWN);
      
      setBackgroundColor (android.graphics.Color.WHITE);
    }
    
    @Override
    protected void onLayout (boolean changed, int left, int top, int right, int bottom)
    {
      super.onLayout (changed, left, top, right, bottom);
      controller.onLayoutCallback (left, top, right, bottom);
    }

    @Override  
    protected void onDisplayHint (int hint)
    {
      super.onDisplayHint (hint);
      controller.onDisplayHintCallback (hint);
    }

    @Override
    protected void onDraw (Canvas c)
    {
      super.onDraw (c);
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
  
  public void loadDataThreadSafe (String in_data, String in_mime_type, String in_encoding, String in_base_url)
  {
    final String data = in_data;
    final String mime_type = in_mime_type;
    final String encoding = in_encoding;
    final String base_url = in_base_url;
    
    UiDispatch.run (view, new UiRunnable () {
      public Object run ()
      {
        view.loadDataWithBaseURL (base_url, data, mime_type, encoding, null); 
        
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
  
  public void stopLoadingThreadSafe ()
  {
    UiDispatch.run (view, new UiRunnable () {
      public Object run ()
      {
        view.stopLoading (); 
        
        return null;
      }
    });
  }    
  
  public void goBackThreadSafe ()
  {
    UiDispatch.run (view, new UiRunnable () {
      public Object run ()
      {
        view.goBack (); 
        
        return null;
      }
    });
  }  
  
  public void goForwardThreadSafe ()
  {
    UiDispatch.run (view, new UiRunnable () {
      public Object run ()
      {
        view.goForward (); 

        return null;
      }
    });
  }

  public boolean canGoBackThreadSafe ()
  {
    Boolean result = (Boolean)UiDispatch.run (view, new UiRunnable () {
      public Object run ()
      {
        return new Boolean (view.canGoBack ());
      }
    });
    
    return result.booleanValue ();
  }
  
  public boolean canGoForwardThreadSafe ()
  {
    Boolean result = (Boolean)UiDispatch.run (view, new UiRunnable () {
      public Object run ()
      {
        return new Boolean (view.canGoForward ());
      }
    });
    
    return result.booleanValue ();
  }
}
