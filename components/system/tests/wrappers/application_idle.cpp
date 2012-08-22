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

  if (count == 1)
    throw xtl::format_operation_exception ("on_idle", "count == 1");
}

void on_application_initialized ()
{
  Application::RegisterEventHandler (ApplicationEvent_OnIdle, &on_idle);
}

void log_print (const char* log, const char* message)
{
  printf ("Log message from '%s': '%s'\n", log, message);
}

int main ()
{
  common::LogFilter filter ("*", &log_print);

  printf ("Results of application_idle_test:\n");

  try
  {
    Application::RegisterEventHandler (ApplicationEvent_OnInitialize, &on_application_initialized);

    Application::Run ();            
  }  
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return Application::GetExitCode ();    
}
