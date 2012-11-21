#include "shared.h"

const char* APP_ID = "271022146306708";

void log_print (const char* stream, const char* message)
{
  printf ("%s: %s\n", stream, message);
  fflush (stdout);
}

void login_callback (social::OperationStatus status, const char* error)
{
  switch (status)
  {
    case social::OperationStatus_Success:
      printf ("Login succeeded\n");
      break;
    case social::OperationStatus_Canceled:
      printf ("Login canceled\n");
      break;
    case social::OperationStatus_Failure:
      printf ("Login failed, error '%s'\n", error);
      break;
    default:
      printf ("Login status unknown\n");
  }

  syslib::Application::Exit (0);
}

int main ()
{
  printf ("Results of login_test:\n");

  try
  {
    common::LogFilter log_filter ("*", &log_print);

    syslib::Window parent_window (syslib::WindowStyle_Overlapped);

    parent_window.SetSize (800, 600);
    parent_window.SetPosition (100, 100);
    parent_window.Show ();

    social::Session session ("Facebook");
    
    common::PropertyMap login_properties;

    login_properties.SetProperty ("AppId", APP_ID);

    session.LogIn (&login_callback, login_properties);

    syslib::Application::Run ();
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
