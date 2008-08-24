#include "shared.h"

int main ()
{
  printf ("Results of output1_test:\n");
  
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
      IAdapter*      adapter    = driver->GetAdapter (i);
      IPropertyList* properties = adapter->GetProperties ();

      printf ("  adapter #%u:\n", i);
      printf ("    name:        '%s'\n", adapter->GetName ());
      printf ("    path:        '%s'\n", adapter->GetPath ());
      printf ("    description: '%s'\n", adapter->GetDescription ());

      for (size_t j=0; j<properties->GetSize (); j++)
        printf ("    %s: '%s'\n", properties->GetKey (j), properties->GetValue (j));

      printf ("    outputs count: %u\n", adapter->GetOutputsCount ());

      for (size_t j=0; j<adapter->GetOutputsCount (); j++)
      {
        IOutput*       output     = adapter->GetOutput (j);
        IPropertyList* properties = output->GetProperties ();

        printf ("    output #%u:\n", j);
        printf ("      name: '%s'\n", output->GetName ());

        for (size_t j=0; j<properties->GetSize (); j++)
          printf ("      %s: '%s'\n", properties->GetKey (j), properties->GetValue (j));        
      }
    }
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }
  
  return 0;
}
