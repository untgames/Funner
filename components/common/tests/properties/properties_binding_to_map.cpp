#include "shared.h"

struct A
{
  int id;
  math::vec3f v;
  
  A () : id (-1), v (1.0f, 2.0f, 3.0f) {}
  
  int Id () const { return id; }
  
  const math::vec3f& Vec () const { return v; }
};

const char* get_const_string ()
{
  return "hello world";
}

void dump_properties (const PropertyMap& properties)
{
  printf ("map has %u properties:\n", properties.Size ());
    
  for (size_t i=0, count=properties.Size (); i<count; i++)
  {  
    stl::string value;
    
    properties.GetProperty (i, value);
    
    printf (" #%u: name='%s', type=%s, value='%s'\n", i, properties.PropertyName (i), get_name (properties.PropertyType (i)), value.c_str ());
  }
}

int main ()
{
  printf ("Results of property_binding_to_map_test:\n");
  
  try
  {
    PropertyBindingMap bindings;
    
    A a;
    
    bindings.AddProperty ("ConstString", &get_const_string);
    bindings.AddProperty ("Id", xtl::bind (&A::Id, &a));
    bindings.AddProperty ("Vec", xtl::bind (&A::Vec, &a));    
    
    PropertyMap map = bindings.CreatePropertyMap ();
    
    dump_properties (map);
  }    
  catch (std::exception& e)
  {
    printf ("%s\n", e.what ());
  }

  return 0;
}
