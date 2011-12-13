package com.untgames.funner.application;

import java.io.*;

public class EngineUtils
{
  public static String getStackTrace (Throwable aThrowable)
  {
    final StringBuilder result  = new StringBuilder ("");
    final String        newLine = System.getProperty ("line.separator");    
    
    result.append (aThrowable.toString ());
    result.append (newLine);

    for (StackTraceElement element : aThrowable.getStackTrace ())
    {
      result.append ("    at ");
      result.append (element);
      result.append (newLine);
    }
    
    return result.toString();
  }
}
