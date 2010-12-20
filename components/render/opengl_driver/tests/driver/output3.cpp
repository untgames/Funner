#include "shared.h"

int main ()
{
  printf ("Results of output3_test:\n");

  try
  {
    xtl::com_ptr<IDriver> driver = DriverManager::FindDriver ("OpenGL");

    if (!driver)
    {
      printf ("OpenGL driver not found\n");
      return 0;
    }

    printf ("Driver:\n");
    printf ("  description:   '%s'\n", driver->GetDescription ());
    printf ("  adapters count: %u\n", driver->GetAdaptersCount ());

    if (driver->GetAdaptersCount () == 0)
    {
      printf ("Adapter not found\n");
      return 0;
    }
        
    IAdapter*      adapter    = driver->GetAdapter (0);
    IPropertyList* properties = adapter->GetProperties ();

    printf ("Adapter [name: '%s'\t", adapter->GetName ());
    printf ("path: '%s'\t", adapter->GetPath ());
    printf ("description: '%s']\n", adapter->GetDescription ());
    
    for (size_t i=0; i<properties->GetSize (); i++)
      printf ("    %s: '%s'\n", properties->GetKey (i), properties->GetValue (i));

    printf ("    outputs count: %u\n", adapter->GetOutputsCount ());

    for (size_t j=0; j<adapter->GetOutputsCount (); j++)
    {
      IOutput*       output     = adapter->GetOutput (j);
      IPropertyList* properties = output->GetProperties ();

      printf ("    output #%u:", j);
      printf (" name: '%s'\t", output->GetName ());

      for (size_t j=0; j<properties->GetSize (); j++)
        printf ("      %s: '%s'\n", properties->GetKey (j), properties->GetValue (j));

      printf ("    modes count: %u\n", output->GetModesCount ());

      OutputModeDesc mode_desc;

      for (size_t k = 0; k < output->GetModesCount (); k++)
      {
        output->GetModeDesc (k, mode_desc);

        printf ("       mode #%u: %dx%dx%d@%d\n", k, mode_desc.width, mode_desc.height, 
                                                     mode_desc.color_bits, mode_desc.refresh_rate);
        output->SetCurrentMode (mode_desc);
      }
    }    
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
