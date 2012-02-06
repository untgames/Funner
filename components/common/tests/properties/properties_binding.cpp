#include "shared.h"

struct A
{
  int id;
  math::vec3f v;
  
  A () : id (0) {}
  
  void SetId (int in_id) { id = in_id; }
  
  int Id () const { return id; }
  
  void SetVec (const math::vec3f& in_v) { v = in_v; }
  
  const math::vec3f& Vec () const { return v; }
};

const char* get_const_string ()
{
  return "hello world";
}

void set_string (const char* s)
{
  printf ("set_string(%s)\n", s);
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
  printf ("Results of property_binding_test:\n");
  
  try
  {
    PropertyBindingMap bindings;
    
    A a;
    
    bindings.AddProperty ("ConstString", &get_const_string);
    bindings.AddProperty ("String", &get_const_string, &set_string);
    bindings.AddProperty ("Id", xtl::bind (&A::Id, &a), xtl::bind (&A::SetId, &a, _1));
    bindings.AddProperty ("Vec", xtl::bind (&A::Vec, &a), xtl::bind (&A::SetVec, &a, _1));    
    
    PropertyMap map;
    
    map.SetProperty ("ConstString", "");
    map.SetProperty ("Id", 0);
    
    printf ("copy to property map:\n");
    
    bindings.CopyToPropertyMap (map);
    
    dump_properties (map);
    
    map.SetProperty ("Id", 3);
    
    bindings.CopyFromPropertyMap (map);
    
    printf ("A.id=%d\n", a.id);
    
    printf ("copy vectors:\n");
    
    map.SetProperty ("Vec", math::vec4f (1.0f));
        
    bindings.CopyToPropertyMap (map);
    
    dump_properties (map);

    map.SetProperty ("Vec", math::vec4f (2.0f));
    
    bindings.CopyFromPropertyMap (map);

    printf ("A.vec=[%.2f %.2f %.2f]\n", a.v.x, a.v.y, a.v.z);
    
    printf ("set string\n");
    
    map.SetProperty ("String", "test");
    
    bindings.CopyFromPropertyMap (map);        
  }    
  catch (std::exception& e)
  {
    printf ("%s\n", e.what ());
  }

  return 0;
}
