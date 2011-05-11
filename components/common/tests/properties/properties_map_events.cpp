#include "shared.h"

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
