package com.untgames.funner.application;

import android.hardware.SensorEventListener;
import android.hardware.Sensor;
import android.hardware.SensorEvent;

public class EngineSensorEventListener implements SensorEventListener
{
  public void onAccuracyChanged (Sensor sensor, int accuracy)
  {
    onAccuracyChangedCallback (sensor, accuracy);
  }
  
  public void onSensorChanged (SensorEvent event)
  {
    onSensorChangedCallback (event.sensor, event.accuracy, event.timestamp, event.values);
  }
  
  private native void onAccuracyChangedCallback (Sensor sensor, int accuracy);
  private native void onSensorChangedCallback (Sensor sensor, int accuracy, long timestamp, final float [] values);  
}
