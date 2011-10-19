#include "shared.h"

int main ()
{
  printf ("Results of screen1_test:\n");
  
  try
  {
    printf ("found %u screens\n", ScreenManager::ScreensCount ());
    
    for (size_t i=0; i<ScreenManager::ScreensCount (); i++)
    {
      Screen screen = ScreenManager::Screen (i);
    
      printf ("  screen[%u]:\n", i);
      printf ("    name:    '%s'\n", screen.Name ());
      printf ("    width:   %u \n", screen.Width ());
      printf ("    height:  %u \n", screen.Height ());
      printf ("    bpp:     %u \n", screen.ColorBits ());
      printf ("    refresh: %u \n", screen.RefreshRate ());
    }
  }  
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
