#include "shared.h"

int main ()
{
  printf ("Results of screen3_test:\n");

  try
  {
    if (!ScreenManager::ScreensCount ())
    {
      printf ("No screens found\n");
      return 0;
    }

    Screen screen (0u);
    
    printf ("change mode...\n");

    ScreenModeDesc mode_desc;

    screen.GetCurrentMode (mode_desc);

    mode_desc.width  = 1024;
    mode_desc.height = 768;

    screen.SetCurrentMode (mode_desc);
    
    printf ("restore mode...\n");
    
    screen.RestoreDefaultMode ();
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
