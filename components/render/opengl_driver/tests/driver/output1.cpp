#include "shared.h"

int main ()
{
  printf ("Results of output1_test:\n");
  
  try
  {
    xtl::com_ptr<IDriver> driver = DriverManager::FindDriver ("OpenGL");
    
    printf ("Driver:\n");
    printf ("  description: '%s'\n", driver->GetDescription ());
    printf ("  outputs count: %u\n", driver->GetOutputsCount ());

    for (size_t i=0; i<driver->GetOutputsCount (); i++)
    {
      IOutput*       output     = driver->GetOutput (i);
      IPropertyList* properties = output->GetProperties ();
      
      printf ("  output #%u:\n", i);
      printf ("    name: '%s'\n", output->GetName ());
      
      for (size_t j=0; j<properties->GetSize (); j++)
        printf ("    %s: '%s'\n", properties->GetKey (j), properties->GetValue (j));
    }
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }
  
  return 0;
}
