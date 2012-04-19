#include <stdio.h>
#include <exception>

#include <xtl/connection.h>
#include <xtl/function.h>

#include <syslib/application.h>
#include <syslib/sensor.h>

using namespace syslib;

void on_update (const SensorEvent& event)
{
  printf ("on_update\n");
}

int main ()
{
  printf ("Results of sensor2_test:\n");

  try
  {   
    Sensor sensor (0);
    
    sensor.RegisterEventHandler (&on_update); 
    
    Application::Run ();
  }  
  catch (std::exception& e)
  {
    printf ("%s\n", e.what ());
  }

  return 0;
}
