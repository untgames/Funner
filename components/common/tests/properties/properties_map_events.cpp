#include "shared.h"

size_t current_hash = 0;
size_t current_layout_hash = 0;

size_t get_correct_hash ()
{
  size_t CORRECT_HASHES_32 [] = { 0x54a83a00, 0x41fe0a00, 0x490df002, 0xf09537d6, 0x584dfd50, 0x0f0012ff, 0xffffffff };
  size_t CORRECT_HASHES_64 [] = { 0x612d49c0, 0x65a5c778, 0x66fa8797, 0xeca9b434, 0x9bfcb969, 0xedaccd16, 0xffffffff };

  return sizeof (void*) == 4 ? CORRECT_HASHES_32 [current_hash++] : CORRECT_HASHES_64 [current_hash++];
}

size_t get_correct_layout_hash ()
{
  size_t CORRECT_HASHES_32 [] = { 0x7d77abc3, 0x4befc2d0, 0xf790e824, 0xba565b70, 0xd7f7fcf3, 0xacba5d65, 0xffffffff };
  size_t CORRECT_HASHES_64 [] = { 0x139eaa9d, 0x6f2d8707, 0x7df2b619, 0x74687881, 0xbaf16e55, 0x8388a354, 0xffffffff };

  return sizeof (void*) == 4 ? CORRECT_HASHES_32 [current_layout_hash++] : CORRECT_HASHES_64 [current_layout_hash++];
}

void event_handler (const PropertyMap& properties, PropertyMapEvent event)
{
  switch (event)
  {
    case PropertyMapEvent_OnUpdate:
      printf ("updated ");
      break;      
    default:
      return;
  }

  printf ("map has %u properties (hash_correct=%d, layout_hash_correct=%d):\n",
    properties.Size (), properties.Hash () == get_correct_hash (), properties.LayoutHash () == get_correct_layout_hash ());
    
  for (size_t i=0, count=properties.Size (); i<count; i++)
  {  
    stl::string value;
    
    properties.GetProperty (i, value);
    
    printf (" #%u: name='%s', type=%s, value='%s'\n", i, properties.PropertyName (i), get_name (properties.PropertyType (i)), value.c_str ());
  }
}

int main ()
{
  printf ("Results of properties_map_events_test:\n");
  
  try
  {
    PropertyMap properties;
    
    properties.RegisterEventHandler (PropertyMapEvent_OnUpdate, xtl::bind (&event_handler, xtl::ref (properties), _1));
        
    printf ("set property 'X'\n");
    properties.SetProperty ("X", "1");
    printf ("set property 'Y'\n");    
    properties.SetProperty ("Y", "3.14");
    printf ("set property 'Z'\n");    
    properties.SetProperty ("Z", "hello world");  
    printf ("remove property 'Y'\n");    
    properties.RemoveProperty ("Y");
    
    printf ("change layout type\n");
    
    PropertyMap copied_properties = properties.Clone ();
    
    copied_properties.RegisterEventHandler (PropertyMapEvent_OnUpdate, xtl::bind (&event_handler, xtl::ref (copied_properties), _1));
    
    properties.Layout ().AddProperty ("ZZZ", PropertyType_Float);
    
    printf ("rename property 'Z' to 'NewZ'\n");    
    properties.SetPropertyName ("Z", "NewZ");

    printf ("change property 'X' type\n");    
    properties.SetPropertyType ("X", PropertyType_Float);
    
    printf ("clear properties\n");    
    properties.Clear ();    
  }
  catch (std::exception& e)
  {
    printf ("exception: %s\n", e.what ());
    throw;
  }
}
