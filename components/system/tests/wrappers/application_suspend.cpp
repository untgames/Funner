#include "shared.h"

static size_t count = 3;

void on_do_events (common::Action& action)
{
  printf ("do_events\n");

  if (!--count)
  {
    action.Cancel ();
  }
}

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
    common::ActionQueue::PushAction (&on_do_events, common::ActionThread_Current, 0, 0);    
    common::ActionQueue::PushAction (&on_exit, common::ActionThread_Background, 2.0);

    Application::Run ();            
  }  
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return Application::GetExitCode ();    
}
