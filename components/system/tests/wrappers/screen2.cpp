#include "shared.h"

int main ()
{
  printf ("Results of screen2_test:\n");

  try
  {
    common::PropertyMap properties;
    
    printf ("screns count: %u\n", ScreenManager::ScreensCount ());

    for (size_t j=0; j<ScreenManager::ScreensCount (); j++)
    {
      Screen screen (j);

      printf ("  screen[%u]:\n", j);
      printf ("    name:    '%s'\n", screen.Name ());
      printf ("    width:   %u \n", screen.Width ());
      printf ("    height:  %u \n", screen.Height ());
      printf ("    bpp:     %u \n", screen.ColorBits ());
      printf ("    refresh: %u \n", screen.RefreshRate ());      
      
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

      printf ("    modes count: %u\n", screen.ModesCount ());

      ScreenModeDesc mode_desc;

      for (size_t k = 0; k < screen.ModesCount (); k++)
      {
        screen.GetMode (k, mode_desc);

        printf ("       mode #%u:\n", k);
        printf ("         width:        %u\n", mode_desc.width);
        printf ("         height:       %u\n", mode_desc.height);
        printf ("         color bits:   %u\n", mode_desc.color_bits);
        printf ("         refresh rate: %u\n", mode_desc.refresh_rate);
      }
    }
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
