#include "shared.h"

void dump_properties (const PropertyMap& properties)
{
  printf ("map has %u properties (hash_correct=%d, layout_hash_correct=%d):\n",
    properties.Size (), properties.Hash () == (sizeof (void*) == 4 ? 0x627782c8 : 0x6266ee16),
    properties.LayoutHash () == (sizeof (void*) == 4 ? 0xb59805b3 : 0x30c8c198));
    
  for (size_t i=0, count=properties.Size (); i<count; i++)
  {  
    stl::string value;
    
    properties.GetProperty (i, value);
    
    printf (" #%u: name='%s', type=%s, value='%s'\n", i, properties.PropertyName (i), get_name (properties.PropertyType (i)), value.c_str ());
  }
}

int main ()
{
  printf ("Results of properties_override_test:\n");
  
  try
  {
    PropertyMap properties;
    
    properties.SetProperty ("WindowStyle", "AAAAAA");
    properties.SetProperty ("CustomCursors", "0");

    properties.SetProperty ("WindowStyle", "AA");
    properties.SetProperty ("CustomCursors", "0");
    
    dump_properties (properties);
  }
  catch (std::exception& e)
  {
    printf ("exception: %s\n", e.what ());
    throw;
  }
}
