#include "shared.h"

static size_t count = 3;

void on_idle ()
{
  printf ("idle\n");

  if (!--count)
  {
    printf ("exiting\n");
    Application::Exit (0);
  }
}

void on_application_initialized ()
{
  Application::RegisterEventHandler (ApplicationEvent_OnIdle, &on_idle);
}

int main ()
{
  printf ("Results of application_idle_test:\n");

  try
  {
    Application::RegisterEventHandler (ApplicationEvent_OnInitialized, &on_application_initialized);

    Application::Run ();            
  }  
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return Application::GetExitCode ();    
}
