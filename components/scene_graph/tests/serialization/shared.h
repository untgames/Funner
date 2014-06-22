#include <stdio.h>

#include <stl/string>

#include <xtl/connection.h>
#include <xtl/function.h>

#include <common/property_map.h>
#include <common/property_binding_map.h>

#include <media/rms/manager.h>
#include <media/font_library.h>

#include <sg/scene_manager.h>

using namespace scene_graph;

namespace
{

float prepare_float (float v)
{
  if (fabs (v) < 0.001f)
    return 0.f;

  return v;
}

/*
    Вспомогательные утилиты вывода состояния объекта
*/

inline void dump (const math::vec3f& v)
{
  printf ("[%+.2f %+.2f %+.2f]", v.x, v.y, v.z);
}

inline void dump (const math::vec4f& v)
{
  printf ("[%+.2f %+.2f %+.2f %+.2f]", v.x, v.y, v.z, v.w);
}

inline void dump (const math::quatf& q)
{
/*  float pitch, yaw, roll;
  
  quat2EulerAngle (q, pitch, yaw, roll);
  
  pitch = fmod (rad2deg (pitch), 360.0f);
  yaw   = fmod (rad2deg (yaw), 360.0f);
  roll  = fmod (rad2deg (roll), 360.0f);

  printf ("[%+.1f %+.1f %+.1f]", pitch, yaw, roll);*/
  
  printf ("[%+.1f %+.1f %+.1f %+.1f]", prepare_float (q.w), prepare_float (q.x), prepare_float (q.y), prepare_float (q.z));
}

inline void dump_position (Node& node)
{
  printf ("local=");
  dump   (node.Position ());
  printf (" world=");
  dump   (node.WorldPosition ());
}

inline void dump_orientation (Node& node)
{
  printf ("local=");
  dump   (node.Orientation ());
  printf (" world=");
  dump   (node.WorldOrientation ());
}

inline void dump_scale (Node& node)
{
  printf ("local=");
  dump   (node.Scale ());
  printf (" world=");
  dump   (node.WorldScale ());
}

inline void dump_properties (const common::PropertyMap& properties)
{
  for (size_t i=0, count=properties.Size (); i<count; i++)
  {  
    stl::string value;
    
    properties.GetProperty (i, value);
    
    printf ("    %s %s='%s'\n", get_name (properties.PropertyType (i)), properties.PropertyName (i), value.c_str ());
  }
}

inline void dump_node (Node& node)
{
  printf ("node '%s':\n", node.Name ());
  printf ("  parent: '%s'\n", node.Parent () ? node.Parent ()->Name () : "");
  
  if (node.Properties ())
  {
    common::PropertyMap& properties = *node.Properties ();
    
    printf ("  properties:\n");
    
    dump_properties (properties);
  }
  
  printf ("  bindings:\n");
  
  dump_properties (node.PropertyBindings ().CreatePropertyMap ());
}

}
