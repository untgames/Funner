#include "shared.h"

#define TEST(X) try { X; }catch (std::exception& e) { printf ("exception: %s\n", e.what ()); }

void dump_dynamic_properties (const VisualModel& node, size_t correct_properties_hash, size_t correct_layout_hash)
{
  if (!node.DynamicShaderProperties ())
  {
    printf ("node has no dynamic shader properties\n");
    return;
  }

  const common::PropertyMap& properties = *node.DynamicShaderProperties ();  
  
  printf ("node has %u dynamic shader properties (hash_correct=%d, structure_hash_correct=%d):\n",
    properties.Size (), properties.Hash () == correct_properties_hash, properties.LayoutHash () == correct_layout_hash);
    
  for (size_t i=0, count=properties.Size (); i<count; i++)
  {  
    stl::string value;
    
    properties.GetProperty (i, value);
    
    printf (" #%u: name='%s', type=%s, value='%s'\n", i, properties.PropertyName (i), get_name (properties.PropertyType (i)), value.c_str ());
  }
}

void dump_static_properties (const VisualModel& node, size_t correct_properties_hash, size_t correct_layout_hash)
{
  if (!node.StaticShaderProperties ())
  {
    printf ("node has no static shader properties\n");
    return;
  }

  const common::PropertyMap& properties = *node.StaticShaderProperties ();  
  
  printf ("node has %u static shader properties (hash_correct=%d, structure_hash_correct=%d):\n",
    properties.Size (), properties.Hash () == correct_properties_hash, properties.LayoutHash () == correct_layout_hash);
    
  for (size_t i=0, count=properties.Size (); i<count; i++)
  {  
    stl::string value;
    
    properties.GetProperty (i, value);
    
    printf (" #%u: name='%s', type=%s, value='%s'\n", i, properties.PropertyName (i), get_name (properties.PropertyType (i)), value.c_str ());
  }
}

void dump_properties (const VisualModel& node, size_t correct_properties_hash_static, size_t correct_layout_hash_static, size_t correct_properties_hash_dynamic, size_t correct_layout_hash_dynamic)
{
  dump_static_properties (node, correct_properties_hash_static, correct_layout_hash_static);
  dump_dynamic_properties (node, correct_properties_hash_dynamic, correct_layout_hash_dynamic);
}

int main ()
{
  printf ("Results of visual_model_shader_properties_test:\n");
  
  try
  {
    TestVisualModel::Pointer node (TestVisualModel::Create ());
    
    dump_properties (*node, 0, 0, 0, 0);
    
    printf ("create properties\n");
    
    node->SetDynamicShaderProperties (common::PropertyMap ());
    
    dump_properties (*node, 0, 0, 0xffffffff, 0xffffffff);
    
    printf ("add dynamic properties\n");
    
    common::PropertyMap& properties1 = *node->DynamicShaderProperties ();

    properties1.SetProperty ("X", "1");
    
    dump_properties (*node, 0, 0, sizeof (void*) == 4 ? 0x54a83a00 : 0x612d49c0, sizeof (void*) == 4 ? 0x7d77abc3 : 0x139eaa9d);

    printf ("add static properties\n");

    node->SetStaticShaderProperties (common::PropertyMap ());

    common::PropertyMap& properties2 = *node->StaticShaderProperties ();

    properties2.SetProperty ("Y", "2");
    
    dump_properties (*node, sizeof (void*) == 4 ? 0xb789e995 : 0x145d5345, sizeof (void*) == 4 ? 0x7e869928 : 0xa7cac198, sizeof (void*) == 4 ? 0x54a83a00 : 0x612d49c0, sizeof (void*) == 4 ? 0x7d77abc3 : 0x139eaa9d);
  }
  catch (std::exception& e)
  {
    printf ("exception: %s\n", e.what ());
    throw;
  }
}
