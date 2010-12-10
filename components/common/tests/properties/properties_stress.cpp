#include "shared.h"

const int PROPERTIES_COUNT = 1000;

void dump_properties (const PropertyMap& properties)
{
  printf ("map has %u properties (hash=%08x, layout_hash=%08x):\n",
    properties.Size (), properties.Hash (), properties.LayoutHash ());
    
  for (size_t i=0, count=properties.Size (); i<count; i++)
  {  
    stl::string value;
    
    properties.GetProperty (i, value);
    
    printf (" #%u: name='%s', type=%s, value='%s'\n", i, properties.PropertyName (i), get_name (properties.PropertyType (i)), value.c_str ());
  }
}

int main ()
{
  printf ("Results of properties_stress_test:\n");
  
  try
  {
    printf ("create properties\n");
    
    PropertyMap properties;

    for (int i=0; i<PROPERTIES_COUNT; i++)
      properties.SetProperty (common::format ("Property%03u", i).c_str (), common::format ("%03u", i).c_str ());
    
    dump_properties (properties);
  }
  catch (std::exception& e)
  {
    printf ("exception: %s\n", e.what ());
    throw;
  }
}
