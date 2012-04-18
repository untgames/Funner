#include <stdio.h>
#include <exception>

#include <syslib/sensor.h>

using namespace syslib;

int main ()
{
  printf ("Results of sensor1_test:\n");

  try
  {
    printf ("system has %u sensors:\n", SensorManager::SensorsCount ());
    
    for (size_t i=0, count=SensorManager::SensorsCount (); i<count; i++)
    {
      Sensor s (i);
      
      printf ("  sensor #%u: name='%s, type='%s', vendor='%s', max_range=%.2f, update_rate=%.2f\n", i, s.Name (), s.Type (), s.Vendor (), s.MaxRange (), s.UpdateRate ());
    }
  }  
  catch (std::exception& e)
  {
    printf ("%s\n", e.what ());
  }

  return 0;
}
