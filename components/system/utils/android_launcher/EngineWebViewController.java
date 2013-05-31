package com.untgames.funner.application;

import android.view.View;
import android.webkit.WebView;
import android.webkit.WebViewClient;
import android.webkit.SslErrorHandler;
import android.net.http.SslError;
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
    
    EngineWebViewClient (EngineWebViewController controller)
    {
      this.controller = controller;
    }
    
    @Override
    public boolean shouldOverrideUrlLoading (WebView view, String url)
    {
      controller.setLoading (false);
        
      UiAsyncBooleanResult result = new UiAsyncBooleanResult ();

      controller.shouldStartLoading (url, result);

      UiDispatch.processMessagesInternally (result);
      
      boolean r = !result.getValue ();
      
      return r;
    }    
    
    @Override
    public void onPageStarted (WebView view, String url, Bitmap favicon)
    {
      controller.setLoading (true);

      super.onPageStarted (view, url, favicon);
      
      controller.onLoadStarted (url);
    }

    @Override
    public void onPageFinished (WebView view, String url) 
    {        
      super.onPageFinished (view, url);

      controller.onLoadFinished ();

      controller.setLoading (false);
    }
    
    @Override
    public void onReceivedError (WebView view, int errorCode, String description, String failingUrl)
    {      
      super.onReceivedError (view, errorCode, description, failingUrl);
      
      controller.onLoadFailed (description);
      
      controller.setLoading (false);
    }
    
    @Override
    public void onReceivedSslError (WebView view, SslErrorHandler handler, SslError error)
    {
      handler.proceed (); // Ignore SSL certificate errors
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

//      setBackgroundColor (android.graphics.Color.WHITE);
      setBackgroundColor (android.graphics.Color.TRANSPARENT);

      setWebViewClient (new EngineWebViewClient (controller));      
      
      requestFocus (View.FOCUS_DOWN);      
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
  volatile long web_view_ref;
  volatile boolean is_loading = false;  

  public EngineWebViewController (Context context, long window_ref)
  {
    view = new EngineWebView (context, this);
    
    initialize (context, view, window_ref);
  }  
  
  public void setWebViewRef (long value)
  {
    web_view_ref = value;
  }
  
  public long getWebViewRef ()
  {
    return web_view_ref;
  }  
  
  void setLoading (boolean state)
  {
    is_loading = state;
  }

  boolean isLoading () { return is_loading; }
  
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
  
  public boolean isLoadingThreadSafe ()
  {
    return isLoading ();
  }

  @Override
  public void setVisibilityThreadSafe (final int visibility)
  {
    super.setVisibilityThreadSafe (visibility);
    
    if (visibility == View.VISIBLE)
    {
      requestFocusThreadSafe ();
    }
  }
  
  public native void onLoadStarted      (String request);
  public native void onLoadFinished     ();
  public native void onLoadFailed       (String error_message);
  public native void shouldStartLoading (String request, UiAsyncBooleanResult result);
}
