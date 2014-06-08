#include "shared.h"

#define TEST(X) try { X; }catch (std::exception& e) { printf ("exception: %s\n", e.what ()); }

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
  printf ("Results of properties_set_buffer_test:\n");
  
  try
  {
    PropertyMap properties1;
        
    properties1.SetProperty ("X", "hello world");
    properties1.SetProperty ("Y", 1);
    properties1.SetProperty ("Z", 3.14f);

    printf ("before:");

    PropertyMap properties2 (properties1.Layout ());

    dump_properties (properties2);

    printf ("after:");

    properties2.SetBufferData (properties1.BufferData ());

    dump_properties (properties2);

    xtl::uninitialized_storage<char> data;

    data.resize (properties1.BufferSize ());

    unsigned int& index = *reinterpret_cast<unsigned int*> (data.data ());

    index = 1000;

    properties2.SetBufferData (data.data ());

    printf ("can't be reached\n");
  }
  catch (std::exception& e)
  {
    printf ("exception: %s\n", e.what ());
  }

  return 0;
}
