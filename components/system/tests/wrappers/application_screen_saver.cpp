#include "shared.h"

void on_exit (common::Action&)
{
  printf ("exit\n");
  Application::Exit (0);
}

int main ()
{
  printf ("Results of application_suspend_test:\n");

  try
  {
    common::ActionQueue::PushAction (&on_exit, common::ActionThread_Main, 70.0);
    
    Application::SetScreenSaverState (false);

    Application::Run ();            
  }  
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return Application::GetExitCode ();    
}
