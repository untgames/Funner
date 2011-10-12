#include "shared.h"

int main ()
{
  printf ("Results of application_screen_saver_test:\n");

  try
  {
    Application::SetScreenSaverState (false);

    Application::Run ();            
  }  
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return Application::GetExitCode ();    
}
