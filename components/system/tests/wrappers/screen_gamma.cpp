#include "shared.h"

void print_gamma_ramp (Color3f table[256])
{
  for (size_t i = 0; i < 256; i++)
    printf ("[%.4f %.4f %.4f]\n", table[i].red, table[i].green, table[i].blue);
}

int main ()
{
  printf ("Results of screen_gamma_test:\n");
  
  try
  {
    if (!ScreenManager::ScreensCount ())
    {
      printf ("No screens found\n");
      return 0;
    }

    Screen screen (0u);

    Color3f table [256], table2[256];

    screen.GetGammaRamp (table);
    
    printf ("Current gamma ramp:\n");
    print_gamma_ramp (table);

    for (size_t i = 0; i < 256; i++)
    {
      table2[i].red   = table[i].red * 0.9f;
      table2[i].green = table[i].green * 0.9f;
      table2[i].blue  = table[i].blue * 0.9f;
    }

    screen.SetGammaRamp (table2);
    screen.GetGammaRamp (table2);
    
    printf ("New gamma ramp:\n");
    print_gamma_ramp (table2);

    screen.SetGammaRamp (table);
    
    printf ("Restored gamma ramp:\n");
    print_gamma_ramp (table);
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }
  
  return 0;
}
