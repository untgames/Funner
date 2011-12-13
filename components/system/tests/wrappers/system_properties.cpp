#include "shared.h"

int main ()
{
  printf ("Results of system_properties_test:\n");
  
  try
  {
    common::PropertyMap properties = Application::SystemProperties ();
    
    printf ("System properties:\n");
    
    for (size_t i=0; i<properties.Size (); i++)
    {
      stl::string value;
      
      properties.GetProperty (i, value);
      
      printf ("  '%s'='%s'\n", properties.PropertyName (i), value.c_str ());
    }
  }  
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
