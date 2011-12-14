#include "shared.h"

int main ()
{
  printf ("Results of driver1_test:\n");
  
  try
  {
    Test test;
    
    printf ("%u drivers registered:\n", DriverManager::DriversCount ());
    
    for (size_t i=0; i<DriverManager::DriversCount (); i++)
    {
      IDriver& driver = *DriverManager::Driver (i);
      
      printf ("  driver #%u: '%s' (%u renderers)\n", i+1, DriverManager::DriverName (i), driver.GetRenderersCount ());
      
      for (size_t j=0; j<driver.GetRenderersCount (); j++)
        printf ("    renderer #%u: '%s'\n", j+1, driver.GetRendererName (j));      
    }
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
