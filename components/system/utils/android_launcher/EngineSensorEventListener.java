package com.untgames.funner.application;

import android.hardware.SensorEventListener;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.content.Context;
import android.view.WindowManager;
import android.view.Display;

public class EngineSensorEventListener implements SensorEventListener
{
  private volatile long sensorRef = 0;
  private Display  display = null;  

  public EngineSensorEventListener (long sensorRef, Context context)
  {
    this.sensorRef = sensorRef;
    this.display   =  ((WindowManager)context.getSystemService (Context.WINDOW_SERVICE)).getDefaultDisplay ();
  }

  public void resetSensorRef ()
  {
    sensorRef = 0;
  }
 
  public void onAccuracyChanged (Sensor sensor, int accuracy)
  {
    if (sensorRef == 0)
      return;      
    
    onAccuracyChangedCallback (sensorRef, sensor, accuracy);
  }
  
  public void onSensorChanged (SensorEvent event)
  {
    if (sensorRef == 0)
      return;          

    onSensorChangedCallback (sensorRef, event.sensor, event.accuracy, event.timestamp, event.values, display.getOrientation ());
  }
  
  private native void onAccuracyChangedCallback (long sensorRef, Sensor sensor, int accuracy);
  private native void onSensorChangedCallback (long sensorRef, Sensor sensor, int accuracy, long timestamp, final float [] values, int orientation);  
}
