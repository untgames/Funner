package com.untgames.funner.application;

import android.view.GestureDetector;
import android.view.SurfaceView;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.View;
import android.view.MotionEvent;
import android.view.KeyEvent;
import android.graphics.Canvas;
import android.graphics.Rect;
import android.widget.FrameLayout;
import android.view.InputDevice;
import android.content.Context;
import android.util.*;
import java.io.*;

public class EngineSurfaceViewController extends EngineViewController
{
  class EngineSurfaceView extends SurfaceView implements SurfaceHolder.Callback 
  {
    private boolean is_surface_created = false;
    private boolean is_visible = false;
    private EngineSurfaceViewController controller;
    
    public EngineSurfaceView (Context context, EngineSurfaceViewController controller)
    {    
      super (context);    
      
      this.controller = controller;            

      getHolder ().addCallback (this);

      getHolder ().setFormat (android.graphics.PixelFormat.RGB_565); //RGBA_8888 not supported on some devices 
        
      if (getHolder ().getSurface () != null && getHolder ().getSurface ().isValid ())
        surfaceCreated (getHolder ());

      setOnTouchListener (controller);
      setOnKeyListener (controller);
      setOnFocusChangeListener (controller);
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

    @Override
    protected void onWindowVisibilityChanged (int visibility)
    {
    	super.onWindowVisibilityChanged (visibility);

    	boolean is_visible_now = visibility == View.VISIBLE;
    	
      if (is_visible == is_visible_now)
      	return;
    	
    	is_visible = is_visible_now;
    			
    	controller.onVisibilityChange (is_visible);
    }
    
    @Override    
    public void surfaceChanged (SurfaceHolder holder, int format, int width, int height)
    {
      controller.onSurfaceChangedCallback (format, width, height);
    }  

    @Override    
    public void surfaceCreated (SurfaceHolder holder)
    {
      if (is_surface_created)
        return;
        
      is_surface_created = true;
        
      controller.onSurfaceCreatedCallback ();
    }
    
    @Override    
    public void surfaceDestroyed (SurfaceHolder holder)
    {
      if (!is_surface_created)
        return;
        
      is_surface_created = false;
      
      controller.onSurfaceDestroyedCallback ();
    }    
  }    
 
  public EngineSurfaceViewController (Context context, long window_ref)
  {
    initialize (context, new EngineSurfaceView (context, this), window_ref);
  }  
}
