#include <cstdio>
#include <exception>

#include <common/property_map.h>
#include <common/strlib.h>

using namespace common;

void dump_properties (const PropertyMap& properties)
{
  printf ("map has %u properties (hash_correct=%d, layout_hash_correct=%d):\n",
    properties.Size (), properties.Hash () == (sizeof (void*) == 4 ? 0xd7cae3c4 : 0x9e6df51b),
    properties.LayoutHash () == (sizeof (void*) == 4 ? 0x86f26cbf : 0xb01cabdd));
    
  for (size_t i=0, count=properties.Size (); i<count; i++)
  {  
    stl::string value;
    
    properties.GetProperty (i, value);
    
    printf (" #%u: name='%s', type=%s, value='%s'\n", i, properties.PropertyName (i), get_name (properties.PropertyType (i)), value.c_str ());
  }
}

int main ()
{
  printf ("Results of properties_parser_test:\n");
  
  try
  {
    PropertyMap properties = parse_init_string ("A=1 B=2");
    
    dump_properties (properties);
  }
  catch (std::exception& e)
  {
    printf ("exception: %s\n", e.what ());
  }
}
