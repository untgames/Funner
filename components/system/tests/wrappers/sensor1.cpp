#include <stdio.h>
#include <exception>

#include <stl/string>

#include <common/property_map.h>

#include <syslib/sensor.h>

using namespace syslib;

int main ()
{
  printf ("Results of sensor1_test:\n");

  try
  {
    printf ("system has %u sensors:\n", SensorManager::SensorsCount ());
    
    common::PropertyMap properties;    
    
    for (size_t i=0, count=SensorManager::SensorsCount (); i<count; i++)
    {
      Sensor s (i);
      
      printf ("  sensor #%u: Name='%s'\n    Type='%s'\n    Vendor='%s'\n    MaxRange=%.2f\n    UpdateRate=%.2f\n", i, s.Name (), s.Type (), s.Vendor (), s.MaxRange (), s.UpdateRate ());      
      
      s.GetProperties (properties);
      
      for (size_t j=0; j<properties.Size (); j++)
      {
        stl::string s;
        
        properties.GetProperty (j, s);
        
        printf ("    %s='%s'\n", properties.PropertyName (j), s.c_str ());
      }
    }
  }  
  catch (std::exception& e)
  {
    printf ("%s\n", e.what ());
  }

  return 0;
}
