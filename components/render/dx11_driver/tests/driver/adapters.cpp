#include "shared.h"

int main ()
{
  printf ("Results of adapters_test:\n");
  
  try
  {
    xtl::com_ptr<IDriver> driver = DriverManager::FindDriver ("OpenGL");
    
    if (!driver)
    {
      printf ("OpenGL driver not found");
      return 0;
    }    
    
    printf ("Driver:\n");
    printf ("  description:   '%s'\n", driver->GetDescription ());
    printf ("  adapters count: %u\n", driver->GetAdaptersCount ());

    for (size_t i=0; i<driver->GetAdaptersCount (); i++)
    {
      IAdapter* adapter = driver->GetAdapter (i);

      printf ("  adapter #%u:\n", i);
      printf ("    name:        '%s'\n", adapter->GetName ());
      printf ("    path:        '%s'\n", adapter->GetPath ());
      printf ("    description: '%s'\n", adapter->GetDescription ());
    }
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }
  
  return 0;
}
