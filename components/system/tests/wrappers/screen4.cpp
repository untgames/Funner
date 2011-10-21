#include "shared.h"

int main ()
{
  printf ("Results of screen4_test:\n");

  try
  {
    if (!ScreenManager::ScreensCount ())
    {
      printf ("No screens found\n");
      return 0;
    }

    Screen screen ((size_t)0);

    ScreenModeDesc mode_desc;

    screen.GetDefaultMode (mode_desc);

    printf ("default mode:\n");
    printf ("  width:        %u\n", mode_desc.width);
    printf ("  height:       %u\n", mode_desc.height);
    printf ("  color bits:   %u\n", mode_desc.color_bits);
    printf ("  refresh rate: %u\n", mode_desc.refresh_rate);
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
