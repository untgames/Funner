#include "shared.h"

int main ()
{
  printf ("Results of output2_test:\n");
  
  try
  {
    IDriver* driver = get_opengl_driver ();
    
    printf ("Driver:\n");
    printf ("  description: '%s'\n", driver->GetDescription ());
    printf ("  outputs count: %u\n", driver->GetOutputsCount ());

    for (size_t i=0; i<driver->GetOutputsCount (); i++)
    {
      IOutput*       output     = driver->GetOutput (i);
      IPropertyList* properties = output->GetProperties ();
      
      printf ("  output #%u: '%s'\n", i, output->GetName ());
    }
    
    if (!driver->GetOutputsCount ())
    {
      printf ("No output devices found\n");
      return 0;
    }
    
    IOutput* output = driver->GetOutput (0);
    
    OutputModeDesc mode_desc;        
    
    output->GetCurrentMode (mode_desc);
    output->SetCurrentMode (mode_desc);
    
    mode_desc.width  = 1024;
    mode_desc.height = 768;

    output->SetCurrentMode (mode_desc);        

    mode_desc.refresh_rate = 13;
    
    output->SetCurrentMode (mode_desc);
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }
  
  return 0;
}
