#include "shared.h"

xtl::connection do_events_connection;

void on_do_events ()
{
  static size_t count = 3;

  printf ("do_events\n");

  if (!--count)
    do_events_connection.disconnect ();
}

bool on_suspend ()
{
  printf ("suspend\n");
    
  Application::Exit (0);

  return true;
}

int main ()
{
  printf ("Results of application_suspend_test:\n");
  
  try
  {
    do_events_connection = Application::RegisterEventHandler   (ApplicationEvent_OnDoEvents, &on_do_events);
    Application::RegisterSuspendHandler (&on_suspend);
    
    Application::Run ();            
  }  
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return Application::GetExitCode ();    
}
