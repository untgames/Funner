package com.untgames.funner.application;

import android.hardware.SensorEventListener;
import android.hardware.Sensor;
import android.hardware.SensorEvent;

public class EngineSensorEventListener implements SensorEventListener
{
  private volatile long sensorRef = 0;

  public EngineSensorEventListener (long sensorRef)
  {
    this.sensorRef = sensorRef;
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

    onSensorChangedCallback (sensorRef, event.sensor, event.accuracy, event.timestamp, event.values);
  }
  
  private native void onAccuracyChangedCallback (long sensorRef, Sensor sensor, int accuracy);
  private native void onSensorChangedCallback (long sensorRef, Sensor sensor, int accuracy, long timestamp, final float [] values);  
}
