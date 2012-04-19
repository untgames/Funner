#include "shared.h"

using namespace syslib;

void on_update (const SensorEvent& event)
{
  printf ("on_update: ");
  
  for (size_t i=0; i<event.values_count; i++)
  {
    if (i)
      printf (", ");
    
    printf ("%.2f", event.data [i]);        
  }
  
  printf ("\n");
}

void log_print (const char* stream, const char* message)
{
  printf ("%s: %s\n", stream, message);
  fflush (stdout);
}

int main ()
{
  printf ("Results of sensor2_test:\n");

  try
  {   
    common::LogFilter log_filter ("*", &log_print);    
    
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
