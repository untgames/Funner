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
  printf ("Results of properties_test:\n");
  
  try
  {
    printf ("create properties\n");
    
    PropertyMap properties1;
    
    dump_properties (properties1);
    
    printf ("add properties\n");
    
    properties1.SetProperty ("X", "1");
    properties1.SetProperty ("Y", "3.14");
    properties1.SetProperty ("Z", "hello world");
    
    dump_properties (properties1);    
    
    printf ("remove properties\n");
    
    properties1.RemoveProperty ("Y");

    dump_properties (properties1);
    
    printf ("clear\n");
    
    properties1.Clear ();
    
    dump_properties (properties1);
    
    printf ("change property name\n");
    
    properties1.SetProperty ("X", "1");
    properties1.SetProperty ("Y", "3.14");
    properties1.SetProperty ("Z", "hello world");
    
    properties1.SetPropertyName ("Z", "NewZ");
    
    TEST (properties1.SetPropertyName ("UnknownProperty", "NewName"));    
    
    dump_properties (properties1);
    
    printf ("change type\n");
    
    TEST (properties1.SetPropertyType ("X", PropertyType_Float));
    TEST (properties1.SetPropertyType ("NewZ", PropertyType_Vector));
    
    dump_properties (properties1);
    
    printf ("clone\n");
    
    PropertyMap properties2 = properties1.Clone ();
    
    dump_properties (properties2);
    
    printf ("is_present\n");
    
    printf ("is_present('%s'): %s\n", "Y", properties2.IsPresent ("Y") ? "true" : "false");
    printf ("is_present('%s'): %s\n", "Y1", properties2.IsPresent ("Y1") ? "true" : "false");
  }
  catch (std::exception& e)
  {
    printf ("exception: %s\n", e.what ());
    throw;
  }
}
