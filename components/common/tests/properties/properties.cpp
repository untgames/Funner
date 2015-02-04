#include "shared.h"

#define TEST(X) try { X; }catch (std::exception& e) { printf ("exception: %s\n", e.what ()); }

void dump_properties (const PropertyMap& properties, size_t correct_hash, size_t correct_layout_hash)
{
  printf ("map has %u properties (hash_correct=%d, layout_hash_correct=%d):\n",
    properties.Size (), properties.Hash () == correct_hash, properties.LayoutHash () == correct_layout_hash);
    
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
    
    dump_properties (properties1, 0xffffffff, 0xffffffff);
    
    printf ("add properties\n");
    
    properties1.SetProperty ("X", "1");
    properties1.SetProperty ("Y", "3.14");
    properties1.SetProperty ("Z", "hello world");
    
    dump_properties (properties1, sizeof (void*) == 4 ? 0x490df002 : 0x719e36a0, sizeof (void*) == 4 ? 0xf790e824 : 0xd5292b98);
    
    printf ("change properties\n");    
    
    properties1.SetProperty ("Y", "3.12");    
    
    dump_properties (properties1, sizeof (void*) == 4 ? 0xb4f27967 : 0x8c61bfc5, sizeof (void*) == 4 ? 0xf790e824 : 0xd5292b98);
    
    printf ("remove properties\n");
    
    properties1.RemoveProperty ("Y");

    dump_properties (properties1, sizeof (void*) == 4 ? 0x0d6abeb3 : 0x344db67a, sizeof (void*) == 4 ? 0xba565b70 : 0x57d72eb0);
    
    printf ("clear\n");
    
    properties1.Clear ();
    
    dump_properties (properties1, 0xffffffff, 0xffffffff);
    
    printf ("change property name\n");
    
    properties1.SetProperty ("X", "1");
    properties1.SetProperty ("Y", "3.14");
    properties1.SetProperty ("Z", "hello world");
    
    properties1.SetPropertyName ("Z", "NewZ");
    
    TEST (properties1.SetPropertyName ("UnknownProperty", "NewName"));    
    
    dump_properties (properties1, sizeof (void*) == 4 ? 0x88c76d7c : 0xfccd9aa4, sizeof (void*) == 4 ? 0x50dbace6 : 0x76e9ab13);
    
    printf ("change type\n");
    
    TEST (properties1.SetPropertyType ("X", PropertyType_Float));
    TEST (properties1.SetPropertyType ("NewZ", PropertyType_Vector));
    
    dump_properties (properties1, sizeof (void*) == 4 ? 0xfcdacd2a : 0x4e6271ef, sizeof (void*) == 4 ? 0x1dea3343 : 0xf95ead3d);
    
    printf ("clone\n");
    
    PropertyMap properties2 = properties1.Clone ();
    
    dump_properties (properties2, sizeof (void*) == 4 ? 0xfcdacd2a : 0x4e6271ef, sizeof (void*) == 4 ? 0x1dea3343 : 0xf95ead3d);
    
    printf ("is_present\n");
    
    printf ("is_present('%s'): %s\n", "Y", properties2.IsPresent ("Y") ? "true" : "false");
    printf ("is_present('%s'): %s\n", "Y1", properties2.IsPresent ("Y1") ? "true" : "false");
  }
  catch (std::exception& e)
  {
    printf ("exception: %s\n", e.what ());
  }

  return 0;
}
