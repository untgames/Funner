#include "shared.h"

void print_gamma_ramp (Color3f table[256])
{
  for (size_t i = 0; i < 256; i++)
    printf ("[%.4f %.4f %.4f]\n", table[i].red, table[i].green, table[i].blue);
}

int main ()
{
  printf ("Results of output_gamma_ramp_test:\n");
  
  try
  {
    xtl::com_ptr<IDriver> driver = DriverManager::FindDriver ("OpenGL");
    
    if (!driver)
    {
      printf ("OpenGL driver not found\n");
      return 0;
    }
    
    printf ("Driver:\n");
    printf ("  description: '%s'\n", driver->GetDescription ());
    
    if (!driver->GetAdaptersCount () || !driver->GetAdapter (0)->GetOutputsCount ())
    {
      printf ("No outputs found\n");
      return 0;
    }

    IOutput *output = driver->GetAdapter (0)->GetOutput (0);
    Color3f table [256], table2[256];

    output->GetGammaRamp (table);
    printf ("Current gamma ramp:\n");
    print_gamma_ramp (table);

    for (size_t i = 0; i < 256; i++)
    {
      table2[i].red   = table[i].red * 0.9f;
      table2[i].green = table[i].green * 0.9f;
      table2[i].blue  = table[i].blue * 0.9f;
    }

    output->SetGammaRamp (table2);
    output->GetGammaRamp (table2);
    printf ("New gamma ramp:\n");
//    print_gamma_ramp (table2);

//    output->SetGammaRamp (table);
    output->GetGammaRamp (table);
    printf ("Restored gamma ramp:\n");
    print_gamma_ramp (table);
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }
  
  return 0;
}
