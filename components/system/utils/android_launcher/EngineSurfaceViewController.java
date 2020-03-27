package com.untgames.funner.application;

import android.graphics.Point;
import android.os.Handler;
import android.view.Display;
import android.view.GestureDetector;
import android.view.SurfaceView;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.View;
import android.view.MotionEvent;
import android.view.KeyEvent;
import android.view.WindowManager;
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
    private static final int CHANGE_SURFACE_SIZE_AFTER_DELAY = 500;
	  
    private boolean is_surface_created = false;
    private boolean is_visible = false;
    private EngineSurfaceViewController controller;
    private float content_scale_factor;
    private Handler change_surface_size_handler;
    private int last_layout_width;
    private int last_layout_height;
    
    public EngineSurfaceView (Context context, EngineSurfaceViewController controller, final float content_scale_factor)
    {    
      super (context);    
      
      this.controller = controller;
      this.content_scale_factor = content_scale_factor;

      getHolder ().addCallback (this);

      getHolder ().setFormat (android.graphics.PixelFormat.RGB_565); //RGBA_8888 not supported on some devices
      
      if (content_scale_factor < 1)
      {
        Display display = ((WindowManager)context.getApplicationContext ().getSystemService (Context.WINDOW_SERVICE)).getDefaultDisplay ();
	      
        Point display_size = new Point ();
	      
        if (android.os.Build.VERSION.SDK_INT >= 17) 
          display.getRealSize (display_size);
        else
          display.getSize (display_size);
	      
        last_layout_width  = display_size.x;
        last_layout_height = display_size.y;
	      
        getHolder ().setFixedSize ((int)(last_layout_width * content_scale_factor), (int)(last_layout_height * content_scale_factor));
      }

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

      int width  = right - left;
      int height = bottom - top;
      
      if (width != last_layout_width || height != last_layout_height)
      {
        //method called multiple times during window creation, so postpone changing surface size for a two frames to avoid unnecessary surface changes 
	
        if (change_surface_size_handler != null)
          change_surface_size_handler.removeCallbacksAndMessages (null);
	      
        change_surface_size_handler = new Handler();
	      
        change_surface_size_handler.postDelayed (new Runnable () {
          @Override
          public void run() {
            getHolder ().setFixedSize ((int)(width * content_scale_factor), (int)(height * content_scale_factor));
            change_surface_size_handler = null;
          }    	  
        }, CHANGE_SURFACE_SIZE_AFTER_DELAY);
	      
        last_layout_width  = width;
        last_layout_height = height;
      }
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
    
    public Surface getSurface ()
    {
      return getHolder ().getSurface ();
    }
  }    
 
  public EngineSurfaceViewController (Context context, final long window_ref, final float content_scale_factor)
  {
    initialize (context, new EngineSurfaceView (context, this, content_scale_factor), window_ref);
  }  
}
