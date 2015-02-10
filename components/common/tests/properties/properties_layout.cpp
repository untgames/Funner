#include "shared.h"

int main ()
{
  printf ("Results of properties_layout_test:\n");
  
  try
  {
    PropertyLayout layout1;
    
    printf ("add properties\n");
    
    layout1.AddProperty ("A", PropertyType_Integer);
    
    size_t index = layout1.AddProperty ("B", PropertyType_Float, 2);
    
    layout1.AddProperty ("C", PropertyType_Matrix);
    
    dump (layout1, sizeof (void*) == 4 ? 0x3cde1c35 : 0x49e0a0e0);
    
    printf ("remove property\n");
    
    layout1.RemoveProperty (index);
    
    dump (layout1, sizeof (void*) == 4 ? 0xd2a0b67b : 0xfa650253);
    
    printf ("change type/elements_count\n");
    
    layout1.SetPropertyType ("A", PropertyType_Vector);
    layout1.SetPropertyElementsCount ("C", PropertyType_Integer);
    
    dump (layout1, sizeof (void*) == 4 ? 0x3bab9723 : 0x6677448b);
    
    PropertyLayout layout2;
    
    layout2.AddProperty ("A", PropertyType_Vector);
    layout2.AddProperty ("C", PropertyType_Integer);    
    
    dump (layout1, sizeof (void*) == 4 ? 0x3bab9723 : 0x6677448b);
    
    printf ("IsPresent('A'): %s\n", layout2.IsPresent ("A") ? "true" : "false");
  }
  catch (std::exception& e)
  {
    printf ("%s\n", e.what ());
  }
}
