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
import android.content.Context;
import android.util.*;
import java.io.*;

public class EngineView extends SurfaceView implements SurfaceHolder.Callback 
{
  private GestureDetector gesture_detector;
    
  class DoubletapListener extends GestureDetector.SimpleOnGestureListener
  {
    private EngineView engine_view;
    
    public DoubletapListener (EngineView in_engine_view)
    {
      engine_view = in_engine_view;
    }
    
    @Override
    public boolean onDoubleTap (MotionEvent event)
    {
      engine_view.onDoubletapCallback (event.getPointerId (0), event.getX (), event.getY ()); 
        
      return true;
    }
  }
  
  public EngineView (Context context)
  {    
    super (context);

    gesture_detector = new GestureDetector (context, new DoubletapListener (this)); 
    
    setFocusable (true);
    setFocusableInTouchMode (true);
    
    getHolder ().addCallback (this);
  }
  
  public Surface getSurfaceThreadSafe ()
  {
    Surface result = (Surface)UiDispatch.run (this, new UiRunnable () {
      public Object run ()
      {
        return getHolder ().getSurface ();
      }
    });
    
    return result;
  }
  
  public void maximizeThreadSafe ()
  {
    UiDispatch.run (this, new UiRunnable () {
      public Object run ()
      {
        setLayoutParams (new FrameLayout.LayoutParams (FrameLayout.LayoutParams.FILL_PARENT, FrameLayout.LayoutParams.FILL_PARENT));
        
        return null;
      }
    });
  }
  
  public int getTopThreadSafe ()
  {
    Integer result = (Integer)UiDispatch.run (this, new UiRunnable () {
      public Object run ()
      {
        return new Integer (getTop ());
      }
    });
    
    return result.intValue ();
  }
    
  public int getLeftThreadSafe ()
  {
    Integer result = (Integer)UiDispatch.run (this, new UiRunnable () {
      public Object run ()
      {
        return new Integer (getLeft ());
      }
    });
    
    return result.intValue ();
  }
  
  public int getWidthThreadSafe ()
  {
    Integer result = (Integer)UiDispatch.run (this, new UiRunnable () {
      public Object run ()
      {
        return new Integer (getWidth ());
      }
    });
    
    return result.intValue ();
  }
  
  public int getHeightThreadSafe ()
  {
    Integer result = (Integer)UiDispatch.run (this, new UiRunnable () {
      public Object run ()
      {
        return new Integer (getHeight ());
      }
    });
    
    return result.intValue ();
  }
  
  public void layoutThreadSafe (final int left, final int top, final int right, final int bottom)
  {
    UiDispatch.run (this, new UiRunnable () {
      public Object run ()
      {
        layout (left, top, right, bottom);
        
        return null;
      }
    });
  }
  
  public int getVisibilityThreadSafe ()
  {
    Integer result = (Integer)UiDispatch.run (this, new UiRunnable () {
      public Object run ()
      {
        return new Integer (getVisibility ());
      }
    });
    
    return result.intValue ();
  }
  
  public void setVisibilityThreadSafe (final int visibility)
  {
    UiDispatch.run (this, new UiRunnable () {
      public Object run ()
      {
        setVisibility (visibility);
        
        return null;
      }
    });
  }  

  public void setBackgroundColorThreadSafe (final int color)
  {
    UiDispatch.run (this, new UiRunnable () {
      public Object run ()
      {
        setBackgroundColor (color);
        
        return null;
      }
    });
  }
  
  public void bringToFrontThreadSafe ()
  {
    UiDispatch.run (this, new UiRunnable () {
      public Object run ()
      {
        bringToFront ();
        
        return null;
      }
    });
  }
  
  public boolean requestFocusThreadSafe ()
  {
    Boolean result = (Boolean)UiDispatch.run (this, new UiRunnable () {
      public Object run ()
      {
        return new Boolean (requestFocus ());
      }
    });
    
    return result.booleanValue ();
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
  protected void onLayout (boolean changed, int left, int top, int right, int bottom)
  {
    onLayoutCallback (left, top, right, bottom);
  }

  @Override  
  protected void onDisplayHint (int hint)
  {
    onDisplayHintCallback (hint);
  }
  
  @Override
  protected void onDraw (Canvas c)
  {
    onDrawCallback ();
  }
  
  @Override
  public boolean onTouchEvent (MotionEvent event)
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
  
  @Override
  public boolean onTrackballEvent (MotionEvent event)
  {
    int pointers_count = event.getPointerCount (),
        action         = event.getAction ();
    
    for (int i=0; i<pointers_count; i++)
    {    
      int pointer_id = event.getPointerId (i);  
      
      onTrackballCallback (pointer_id, action, event.getX (i), event.getY (i)); 
    }
    
    return true;
  }  
  
  @Override
  public boolean onKeyDown (int keyCode, KeyEvent event)
  {
    int     action           = event.getAction ();
    boolean is_alt_pressed   = event.isAltPressed (),
            is_shift_pressed = event.isShiftPressed (),
            is_sym_pressed   = event.isSymPressed ();
            
    onKeyCallback (keyCode, action, is_alt_pressed, is_shift_pressed, is_sym_pressed);

    return super.onKeyDown (keyCode, event);
  }
  
  @Override
  public boolean onKeyUp (int keyCode, KeyEvent event)
  {
    int     action           = event.getAction ();
    boolean is_alt_pressed   = event.isAltPressed (),
            is_shift_pressed = event.isShiftPressed (),
            is_sym_pressed   = event.isSymPressed ();
            
    onKeyCallback (keyCode, action, is_alt_pressed, is_shift_pressed, is_sym_pressed);

    return super.onKeyUp (keyCode, event);
  }
  
  @Override protected void onFocusChanged (boolean gainFocus, int direction, Rect previouslyFocusedRect)
  {
    onFocusCallback (gainFocus);
  }
  
  public void surfaceChanged (SurfaceHolder holder, int format, int width, int height)
  {
    onSurfaceChangedCallback (format, width, height);
  }  

  public void surfaceCreated (SurfaceHolder holder)
  {
    onSurfaceCreatedCallback ();
  }
  
  public void surfaceDestroyed (SurfaceHolder holder)
  {
    onSurfaceDestroyedCallback ();
  }
  
  public void surfaceRedrawNeeded (SurfaceHolder holder)
  {
    onDrawCallback ();
  }
}
