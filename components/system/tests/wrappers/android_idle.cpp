#include "shared.h"

void on_idle ()
{
  printf ("idle\n");
  fflush (stdout);

  Application::Sleep (500);
}

void on_application_initialized ()
{
  Application::RegisterEventHandler (ApplicationEvent_OnIdle, &on_idle);
}

void on_pause ()
{
  printf ("paused\n");
  fflush (stdout);
}

void on_resume ()
{
  printf ("resumed\n");
  fflush (stdout);
}

int main ()
{
  printf ("Results of android_idle_test:\n");

  try
  {
    Application::RegisterEventHandler (ApplicationEvent_OnPause, &on_pause);
    Application::RegisterEventHandler (ApplicationEvent_OnResume, &on_resume);
    Application::RegisterEventHandler (ApplicationEvent_OnInitialize, &on_application_initialized);

    Application::Run ();            
  }  
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return Application::GetExitCode ();    
}
