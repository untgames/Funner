#include "shared.h"

void on_any_notification (const char* notification)
{
  printf ("on_any_notification '%s'\n", notification);
}

void on_my_notification (const char* notification)
{
  printf ("on_my_notification '%s'\n", notification);

  Application::Exit (0);
}

void on_application_initialized ()
{
  Application::PostNotification ("Application initialized");
  Application::PostNotification ("my notification!");
}

int main ()
{
  printf ("Results of application_exit_test:\n");
  
  try
  {
    Application::RegisterNotificationHandler ("*", &on_any_notification);
    Application::RegisterNotificationHandler ("my *", &on_my_notification);
    Application::RegisterEventHandler (ApplicationEvent_OnInitialize, &on_application_initialized);

    Application::Run ();        
  }  
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return Application::GetExitCode ();    
}
