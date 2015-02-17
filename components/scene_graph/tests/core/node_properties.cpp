#include "shared.h"

#define TEST(X) try { X; }catch (std::exception& e) { printf ("exception: %s\n", e.what ()); }

void dump_properties (const Node& node, size_t correct_properties_hash, size_t correct_layout_hash)
{
  if (!node.Properties ())
  {
    printf ("node has no properties\n");
    return;
  }

  const common::PropertyMap& properties = *node.Properties ();  
  
  printf ("node has %u properties (hash_correct=%d, structure_hash_correct=%d):\n",
    properties.Size (), properties.Hash () == correct_properties_hash, properties.LayoutHash () == correct_layout_hash);
    
  for (size_t i=0, count=properties.Size (); i<count; i++)
  {  
    stl::string value;
    
    properties.GetProperty (i, value);
    
    printf (" #%u: name='%s', type=%s, value='%s'\n", i, properties.PropertyName (i), get_name (properties.PropertyType (i)), value.c_str ());
  }
}

int main ()
{
  printf ("Results of node_properties_test:\n");
  
  try
  {
    Node::Pointer node (Node::Create ());
    
    dump_properties (*node, 0xffffffff, 0xffffffff);
    
    printf ("create properties\n");
    
    node->SetProperties (common::PropertyMap ());
    
    dump_properties (*node, 0xffffffff, 0xffffffff);
    
    printf ("add properties\n");
    
    common::PropertyMap& properties1 = *node->Properties ();

    properties1.SetProperty ("X", "1");
    properties1.SetProperty ("Y", "3.14");
    properties1.SetProperty ("Z", "hello world");
    
    dump_properties (*node, sizeof (void*) == 4 ? 0x490df002 : 0x66fa8797, sizeof (void*) == 4 ? 0xf790e824 : 0x7df2b619);
    
    printf ("remove properties\n");
    
    properties1.RemoveProperty ("Y");

    dump_properties (*node, sizeof (void*) == 4 ? 0xf09537d6 : 0xeca9b434, sizeof (void*) == 4 ? 0xba565b70 : 0x74687881);
    
    printf ("clear\n");
    
    properties1.Clear ();
    
    dump_properties (*node, 0xffffffff, 0xffffffff);
    
    printf ("change property name\n");
    
    properties1.SetProperty ("X", "1");
    properties1.SetProperty ("Y", "3.14");
    properties1.SetProperty ("Z", "hello world");
    
    properties1.SetPropertyName ("Z", "NewZ");
    
    TEST (properties1.SetPropertyName ("UnknownProperty", "NewName"));    
    
    dump_properties (*node, sizeof (void*) == 4 ? 0x88c76d7c : 0x18807abf, sizeof (void*) == 4 ? 0x50dbace6 : 0xf52e246e);
    
    printf ("change type\n");
    
    TEST (properties1.SetPropertyType ("X", common::PropertyType_Float));
    TEST (properties1.SetPropertyType ("NewZ", common::PropertyType_Vector));
    
    dump_properties (*node, sizeof (void*) == 4 ? 0xfcdacd2a : 0x1ad6122a, sizeof (void*) == 4 ? 0x1dea3343 : 0x93e44cba);
    
    printf ("clone\n");
    
    common::PropertyMap properties2 = properties1.Clone ();
    
    node->SetProperties (0);
    node->SetProperties (properties2);
    
    dump_properties (*node, sizeof (void*) == 4 ? 0xfcdacd2a : 0x1ad6122a, sizeof (void*) == 4 ? 0x1dea3343 : 0x93e44cba);
    
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
