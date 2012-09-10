#include "shared.h"

int main ()
{
  printf ("Results of screen1_test:\n");
  
  try
  {
    printf ("found %u screens\n", ScreenManager::ScreensCount ());
    
    common::PropertyMap properties;    
    
    for (size_t i=0; i<ScreenManager::ScreensCount (); i++)
    {
      Screen screen = ScreenManager::Screen (i);
    
      printf ("  screen[%u]:\n", i);
      printf ("    name:    '%s'\n", screen.Name ());
      printf ("    width:   %u \n", screen.Width ());
      printf ("    height:  %u \n", screen.Height ());
      printf ("    bpp:     %u \n", screen.ColorBits ());
      printf ("    refresh: %u \n", screen.RefreshRate ());      
      printf ("    xdpi:    %u \n", screen.Xdpi ());
      printf ("    ydpi:    %u \n", screen.Ydpi ());
      
      screen.GetProperties (properties);
      
      if (properties.Size ())
      {      
        printf ("    has %u properties:\n", properties.Size ());
        
        for (size_t j=0; j<properties.Size (); j++)
        {
          stl::string value;
          
          properties.GetProperty (j, value);
          
          printf ("      %s='%s'\n", properties.PropertyName (j), value.c_str ());
        }
      }
    }
  }  
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
