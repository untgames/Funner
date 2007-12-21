#include "shared.h"

int main ()
{
  printf ("Results of device_info_test:\n");
  
  try
  {
    Test test (L"OpenGL device test window (device_info)");

    printf ("Device: '%s'\n", test.device->GetName ());

    IPropertyList* properties = test.device->GetProperties ();

    for (size_t j=0; j<properties->GetSize (); j++)
      printf ("  %s: '%s'\n", properties->GetKey (j), properties->GetValue (j));
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }
  
  return 0;
}
