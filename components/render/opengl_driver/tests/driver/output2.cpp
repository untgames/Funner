#include "shared.h"

int main ()
{
  printf ("Results of output2_test:\n");
  
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
      printf ("    outputs count: %u\n", adapter->GetOutputsCount ());

      for (size_t j=0; j<adapter->GetOutputsCount (); j++)
      {
        IOutput* output = adapter->GetOutput (j);

        printf ("    output #%u:\n", j);
        printf ("      name: '%s'\n", output->GetName ());
      }
    }

    if (!driver->GetAdaptersCount () || !driver->GetAdapter (0)->GetOutputsCount ())
    {
      printf ("No output devices found\n");
      return 0;
    }
    
    IOutput* output = driver->GetAdapter (0)->GetOutput (0);

    OutputModeDesc mode_desc;
    
    output->GetCurrentMode (mode_desc);
//    output->SetCurrentMode (mode_desc);

    mode_desc.width  = 1024;
    mode_desc.height = 768;

    output->SetCurrentMode (mode_desc);        

//    mode_desc.refresh_rate = 13;
    
//    output->SetCurrentMode (mode_desc);
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }
  
  return 0;
}
