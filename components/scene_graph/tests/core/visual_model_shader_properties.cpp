#include "shared.h"

#define TEST(X) try { X; }catch (std::exception& e) { printf ("exception: %s\n", e.what ()); }

void dump_dynamic_properties (const VisualModel& node)
{
  if (!node.DynamicShaderProperties ())
  {
    printf ("node has no dynamic shader properties\n");
    return;
  }

  const common::PropertyMap& properties = *node.DynamicShaderProperties ();  
  
  printf ("node has %u dynamic shader properties (hash=%08x, structure_hash=%08x):\n",
    properties.Size (), properties.Hash (), properties.LayoutHash ());    
    
  for (size_t i=0, count=properties.Size (); i<count; i++)
  {  
    stl::string value;
    
    properties.GetProperty (i, value);
    
    printf (" #%u: name='%s', type=%s, value='%s'\n", i, properties.PropertyName (i), get_name (properties.PropertyType (i)), value.c_str ());
  }
}

void dump_static_properties (const VisualModel& node)
{
  if (!node.StaticShaderProperties ())
  {
    printf ("node has no static shader properties\n");
    return;
  }

  const common::PropertyMap& properties = *node.StaticShaderProperties ();  
  
  printf ("node has %u static shader properties (hash=%08x, structure_hash=%08x):\n",
    properties.Size (), properties.Hash (), properties.LayoutHash ());    
    
  for (size_t i=0, count=properties.Size (); i<count; i++)
  {  
    stl::string value;
    
    properties.GetProperty (i, value);
    
    printf (" #%u: name='%s', type=%s, value='%s'\n", i, properties.PropertyName (i), get_name (properties.PropertyType (i)), value.c_str ());
  }
}

void dump_properties (const VisualModel& node)
{
  dump_static_properties (node);
  dump_dynamic_properties (node);
}

int main ()
{
  printf ("Results of visual_model_shader_properties_test:\n");
  
  try
  {
    TestVisualModel::Pointer node (TestVisualModel::Create ());
    
    dump_properties (*node);
    
    printf ("create properties\n");
    
    node->SetDynamicShaderProperties (common::PropertyMap ());
    
    dump_properties (*node);
    
    printf ("add dynamic properties\n");
    
    common::PropertyMap& properties1 = *node->DynamicShaderProperties ();

    properties1.SetProperty ("X", "1");
    
    dump_properties (*node);        

    printf ("add static properties\n");

    node->SetStaticShaderProperties (common::PropertyMap ());

    common::PropertyMap& properties2 = *node->StaticShaderProperties ();

    properties2.SetProperty ("Y", "2");
    
    dump_properties (*node);        
  }
  catch (std::exception& e)
  {
    printf ("exception: %s\n", e.what ());
    throw;
  }
}
