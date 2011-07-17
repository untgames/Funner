#include "shared.h"

void print_log (const char* stream, const char* message)
{
  printf ("%s:%s\n", stream, message);
}

int main ()
{
  printf ("Results of output1_test:\n");

  try
  {
    common::LogFilter filter ("*", &print_log);
  
    xtl::com_ptr<IDriver> driver = DriverManager::FindDriver ("OpenGL");

    if (!driver)
    {
      printf ("OpenGL driver not found\n");
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

        printf ("    modes count: %u\n", output->GetModesCount ());

        OutputModeDesc mode_desc;

        for (size_t k = 0; k < output->GetModesCount (); k++)
        {
          output->GetModeDesc (k, mode_desc);

          printf ("       mode #%u:\n", k);
          printf ("         width:        %u\n", mode_desc.width);
          printf ("         height:       %u\n", mode_desc.height);
          printf ("         color bits:   %u\n", mode_desc.color_bits);
          printf ("         refresh rate: %u\n", mode_desc.refresh_rate);
        }
      }
    }
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
