#include "shared.h"

void on_do_events ()
{
  printf ("do_events\n");
}

bool on_suspend ()
{
  static size_t count = 3;
   
  if (!--count)
  {
    printf ("suspend\n");    
    
    Application::Exit (0);

    return true;
  }  
  
  return false;
}

int main ()
{
  printf ("Results of application_suspend_test:\n");
  
  try
  {
    Application::RegisterEventHandler   (ApplicationEvent_OnDoEvents, &on_do_events);
    Application::RegisterSuspendHandler (&on_suspend);
    
    Application::Run ();            
  }  
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return Application::GetExitCode ();    
}
