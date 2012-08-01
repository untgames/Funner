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

public class EngineViewController implements View.OnTouchListener, View.OnKeyListener, View.OnFocusChangeListener//, View.OnGenericMotionListener
{
  private GestureDetector gesture_detector;
    
  class DoubletapListener extends GestureDetector.SimpleOnGestureListener
  {
    private EngineViewController controller;
    
    public DoubletapListener (EngineViewController controller)
    {
      this.controller = controller;
    }
    
    @Override
    public boolean onDoubleTap (MotionEvent event)
    {
      controller.onDoubletapCallback (event.getPointerId (0), event.getX (), event.getY ()); 

      return true;
    }
  }

  class EngineSurfaceView extends SurfaceView implements SurfaceHolder.Callback 
  {
    private boolean is_surface_created = false;
    private EngineViewController controller;
    
    public EngineSurfaceView (Context context, EngineViewController controller)
    {    
      super (context);    
      
      this.controller = controller;            

      getHolder ().addCallback (this);

      if (getHolder ().getSurface () != null && getHolder ().getSurface ().isValid ())
        surfaceCreated (getHolder ());
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

  private View          view;
  private volatile long window_ref;
  
  private void initialize (View view, long window_ref)
  {    
    this.window_ref = window_ref;
    this.view = view;

    gesture_detector = new GestureDetector (new DoubletapListener (this)); 

    view.setFocusable (true);
    view.setFocusableInTouchMode (true);
    
    view.setOnTouchListener (this);
    view.setOnKeyListener (this);
    view.setOnFocusChangeListener (this);
//    view.setOnGenericMotionListener (this);    
  }
  
  public enum ViewType
  {
    SURFACE_VIEW,
    WEB_VIEW
  }
  
  public EngineViewController (Context context, ViewType type, long window_ref)
  {
    switch (type)
    {
      case SURFACE_VIEW:
        initialize (new EngineSurfaceView (context, this), window_ref);
        break;
      case WEB_VIEW:
        break;
    }
  }
  
  public View getView ()
  {
    return view;
  }
  
  public long getWindowRef ()
  {
    return window_ref;
  }
  
  public void removeFromParentWindowThreadSafe ()
  {
    UiDispatch.run (view, new UiRunnable () {
      public Object run ()
      {
        view.setVisibility (View.GONE);

        return null;
      }
    });
  }
  
  public Surface getSurfaceThreadSafe ()
  {
    Surface result = (Surface)UiDispatch.run (view, new UiRunnable () {
      public Object run ()
      {
        if (view instanceof SurfaceView)
          return ((SurfaceView)view).getHolder ().getSurface ();
          
        return null;
      }
    });
    
    return result;
  }
  
  public void maximizeThreadSafe ()
  {
    UiDispatch.run (view, new UiRunnable () {
      public Object run ()
      {
        view.setLayoutParams (new FrameLayout.LayoutParams (FrameLayout.LayoutParams.FILL_PARENT, FrameLayout.LayoutParams.FILL_PARENT));
        
        return null;
      }
    });
  }
  
  public int getTopThreadSafe ()
  {
    Integer result = (Integer)UiDispatch.run (view, new UiRunnable () {
      public Object run ()
      {
        return new Integer (view.getTop ());
      }
    });
    
    return result.intValue ();
  }
    
  public int getLeftThreadSafe ()
  {
    Integer result = (Integer)UiDispatch.run (view, new UiRunnable () {
      public Object run ()
      {
        return new Integer (view.getLeft ());
      }
    });
    
    return result.intValue ();
  }
  
  public int getWidthThreadSafe ()
  {
    Integer result = (Integer)UiDispatch.run (view, new UiRunnable () {
      public Object run ()
      {
        return new Integer (view.getWidth ());
      }
    });
    
    return result.intValue ();
  }
  
  public int getHeightThreadSafe ()
  {
    Integer result = (Integer)UiDispatch.run (view, new UiRunnable () {
      public Object run ()
      {
        return new Integer (view.getHeight ());
      }
    });
    
    return result.intValue ();
  }
  
  public void layoutThreadSafe (final int left, final int top, final int right, final int bottom)
  {
    UiDispatch.run (view, new UiRunnable () {
      public Object run ()
      {
        view.layout (left, top, right, bottom);
        
        return null;
      }
    });
  }
  
  public int getVisibilityThreadSafe ()
  {
    Integer result = (Integer)UiDispatch.run (view, new UiRunnable () {
      public Object run ()
      {
        return new Integer (view.getVisibility ());
      }
    });
    
    return result.intValue ();
  }
  
  public void setVisibilityThreadSafe (final int visibility)
  {
    UiDispatch.run (view, new UiRunnable () {
      public Object run ()
      {
        view.setVisibility (visibility);
        
        return null;
      }
    });
  }  

  public void setBackgroundColorThreadSafe (final int color)
  {
    UiDispatch.run (view, new UiRunnable () {
      public Object run ()
      {
        view.setBackgroundColor (color);
        
        return null;
      }
    });
  }
  
  public void bringToFrontThreadSafe ()
  {
    UiDispatch.run (view, new UiRunnable () {
      public Object run ()
      {
        view.bringToFront ();
        
        return null;
      }
    });
  }
  
  public boolean requestFocusThreadSafe ()
  {
    Boolean result = (Boolean)UiDispatch.run (view, new UiRunnable () {
      public Object run ()
      {
        return new Boolean (view.requestFocus ());
      }
    });
    
    return result.booleanValue ();
  }
  
  public void postInvalidate ()
  {
    view.postInvalidate ();
  }
  
  private native void onLayoutCallback           (int left, int top, int right, int bottom);
  private native void onDisplayHintCallback      (int hint);
  private native void onDrawCallback             ();
  private native void onTouchCallback            (int pointerId, int action, float x, float y);
  private native void onDoubletapCallback        (int pointerId, float x, float y);
  private native void onTrackballCallback        (int pointerId, int action, float x, float y);
  private native void onKeyCallback              (int keycode, int action, boolean alt_pressed, boolean shift_pressed, boolean is_sym_pressed);
  private native void onFocusCallback            (boolean focusGained);
  private native void onSurfaceCreatedCallback   ();
  private native void onSurfaceChangedCallback   (int format, int width, int height);  
  private native void onSurfaceDestroyedCallback ();  
  
  @Override
  public boolean onTouch (View v, MotionEvent event)
  {
    int pointers_count = event.getPointerCount (),
        action         = event.getAction ();
    
    for (int i=0; i<pointers_count; i++)
    {    
      int pointer_id = event.getPointerId (i);  
      
      onTouchCallback (pointer_id, action, event.getX (i), event.getY (i)); 
    }
    
    gesture_detector.onTouchEvent (event);
    
    return true;
  }
  
/*  @Override
  public boolean onGenericMotion (View v, MotionEvent event)
  {
    if (event.getSource () != InputDevice.SOURCE_CLASS_TRACKBALL)
      return false;
    
    int pointers_count = event.getPointerCount (),
        action         = event.getAction ();

    for (int i=0; i<pointers_count; i++)
    {    
      int pointer_id = event.getPointerId (i);  

      onTrackballCallback (pointer_id, action, event.getX (i), event.getY (i)); 
    }
    
    return true;
  }*/
  
  @Override
  public boolean onKey (View v, int keyCode, KeyEvent event)
  {
    int     action           = event.getAction ();
    boolean is_alt_pressed   = event.isAltPressed (),
            is_shift_pressed = event.isShiftPressed (),
            is_sym_pressed   = event.isSymPressed ();
            
    onKeyCallback (keyCode, action, is_alt_pressed, is_shift_pressed, is_sym_pressed);

    return true;
  }  
  
  @Override
  public void onFocusChange (View v, boolean has_focus)
  {
    onFocusCallback (has_focus);
  }    
}
