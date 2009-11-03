#include "shared.h"

#define TEST(X) try { X; }catch (std::exception& e) { printf ("exception: %s\n", e.what ()); }

void dump_properties (const Node& node)
{
  if (!node.Properties ())
  {
    printf ("node has no properties\n");
    return;
  }

  const NodeProperties& properties = *node.Properties ();  
  
  printf ("node has %u properties (hash=%08x, structure_hash=%08x, data_hash=%08x):\n",
    properties.Size (), properties.Hash (), properties.StructureHash (), properties.DataHash ());    
    
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
    
    dump_properties (*node);
    
    printf ("create properties\n");
    
    node->SetProperties (NodeProperties::Create ());
    
    dump_properties (*node);
    
    printf ("add properties\n");
    
    NodeProperties& properties1 = *node->Properties ();

    properties1.SetProperty ("X", "1");
    properties1.SetProperty ("Y", "3.14");
    properties1.SetProperty ("Z", "hello world");
    
    dump_properties (*node);    
    
    printf ("remove properties\n");
    
    properties1.Remove ("Y");

    dump_properties (*node);
    
    printf ("clear\n");
    
    properties1.Clear ();
    
    dump_properties (*node);
    
    printf ("change property name\n");
    
    properties1.SetProperty ("X", "1");
    properties1.SetProperty ("Y", "3.14");
    properties1.SetProperty ("Z", "hello world");
    
    properties1.SetPropertyName ("Z", "NewZ");
    
    TEST (properties1.SetPropertyName ("UnknownProperty", "NewName"));    
    
    dump_properties (*node);
    
    printf ("change type\n");
    
    TEST (properties1.SetPropertyType ("X", NodePropertyType_Float));
    TEST (properties1.SetPropertyType ("NewZ", NodePropertyType_Vector));
    
    dump_properties (*node);
    
    printf ("clone\n");
    
    NodeProperties::Pointer properties2 = properties1.Clone ();
    
    node->SetProperties (0);
    node->SetProperties (properties2);
    
    dump_properties (*node);
    
    printf ("is_present\n");
    
    printf ("is_present('%s'): %s\n", "Y", properties2->IsPresent ("Y") ? "true" : "false");
    printf ("is_present('%s'): %s\n", "Y1", properties2->IsPresent ("Y1") ? "true" : "false");
  }
  catch (std::exception& e)
  {
    printf ("exception: %s\n", e.what ());
    throw;
  }
}
