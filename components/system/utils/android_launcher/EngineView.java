package com.untgames.funner.application;

import android.view.SurfaceView;
import android.view.View;
import android.widget.FrameLayout;
import android.content.Context;
import android.util.*;
import java.io.*;

public class EngineView extends SurfaceView
{
  public EngineView (Context context)
  {    
    super (context);
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
    final View view = this;
    
    Integer result = (Integer)UiDispatch.run (this, new UiRunnable () {
      public Object run ()
      {
        return new Integer (view.getTop ());
      }
    });
    
    return result.intValue ();
  }
    
  public int getLeftThreadSafe ()
  {
    final View view = this;
    
    Integer result = (Integer)UiDispatch.run (this, new UiRunnable () {
      public Object run ()
      {
        return new Integer (view.getLeft ());
      }
    });
    
    return result.intValue ();
  }
  
  public int getWidthThreadSafe ()
  {
    final View view = this;
    
    Integer result = (Integer)UiDispatch.run (this, new UiRunnable () {
      public Object run ()
      {
        return new Integer (view.getWidth ());
      }
    });
    
    return result.intValue ();
  }
  
  public int getHeightThreadSafe ()
  {
    final View view = this;
    
    Integer result = (Integer)UiDispatch.run (this, new UiRunnable () {
      public Object run ()
      {
        return new Integer (view.getHeight ());
      }
    });
    
    return result.intValue ();
  }
  
  public void layoutThreadSafe (final int left, final int top, final int right, final int bottom)
  {
    final View view = this;
    
    UiDispatch.run (this, new UiRunnable () {
      public Object run ()
      {
        view.layout (left, top, right, bottom);
        
        return null;
      }
    });
  }
  
  public int getVisibilityThreadSafe ()
  {
    final View view = this;
    
    Integer result = (Integer)UiDispatch.run (this, new UiRunnable () {
      public Object run ()
      {
        return new Integer (view.getVisibility ());
      }
    });
    
    return result.intValue ();
  }
  
  public void setVisibilityThreadSafe (final int visibility)
  {
    final View view = this;
    
    UiDispatch.run (this, new UiRunnable () {
      public Object run ()
      {
        view.setVisibility (visibility);
        
        return null;
      }
    });
  }  

  public void setBackgroundColorThreadSafe (final int color)
  {
    final View view = this;
    
    UiDispatch.run (this, new UiRunnable () {
      public Object run ()
      {
        view.setBackgroundColor (color);
        
        return null;
      }
    });
  }     
}
