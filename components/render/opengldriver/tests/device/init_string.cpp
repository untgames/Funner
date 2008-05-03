#include "shared.h"

int main ()
{
  printf ("Results of init_string_test:\n");
  
  try
  {
    Test test (L"OpenGL device test window (init_string)", "require='GL_ARB_texture_non_power_of_two' disable='GL_EXT_blend_*'");

    printf ("Device: '%s'\n", test.device->GetName ());

    IPropertyList* properties = test.device->GetProperties ();

    for (size_t j=0; j<properties->GetSize (); j++)
      printf ("  %s: '%s'\n", properties->GetKey (j), properties->GetValue (j));
  }
  catch (std::exception& exception)
  {
    printf ("%s\n", exception.what ());
  }  

  return 0;
}
