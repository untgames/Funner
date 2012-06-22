#include "shared.h"

void check (const InputZoneModel& zone, NodeTransformSpace space, const math::vec3f& start, const math::vec3f& dir)
{
  float       ray_intersection_distance = 0.0f, ray_to_zone_distance = -1.0f;
  math::vec2f intersection_point;
  size_t      zone_index = 0;

  bool intersected = zone.IsIntersected (space, start, start + dir, zone_index,
    ray_intersection_distance, ray_to_zone_distance, intersection_point);
    
  const char* space_str = "";
  
  switch (space)
  {
    case NodeTransformSpace_Local:  space_str = "local"; break;
    case NodeTransformSpace_Parent: space_str = "parent"; break;
    case NodeTransformSpace_World:  space_str = "world"; break;        
  }
    
  printf ("%s: space=%s, ray_start=[%.3f, %.3f, %.3f], ray_dir=[%.3f, %.3f, %.3f], ray_distance=%.3f, edge_distance=%.3f, zone_index=%u, point=[%.3f, %.3f]\n",
    intersected ? " in" : "out", space_str, start.x, start.y, start.z, dir.x, dir.y, dir.z, ray_intersection_distance, ray_to_zone_distance,
    zone_index, intersection_point.x, intersection_point.y);
}

int main ()
{
  printf ("Results of input_intersection:\n");
  
  Node::Pointer parent = Node::Create ();
  
  parent->SetPosition (0.0f, -0.5f, -1.0f);
  parent->SetScale    (2.0f, 2.0f, 1.0f);  
  
  InputZone::Pointer zone (InputZone::Create ());
  
  zone->BindToParent (*parent);
    
  check (*zone, NodeTransformSpace_Local, math::vec3f (0.0f, 0.0f, -1.0f), math::vec3f (0.0f, 0.0f, 2.0f));  
  check (*zone, NodeTransformSpace_Local, math::vec3f (0.5f, 0.51f, -1.0f), math::vec3f (0.0f, 0.0f, 2.0f));  
  check (*zone, NodeTransformSpace_Local, math::vec3f (0.0f, 0.0f, -1.0f), math::vec3f (0.0f, 0.0f, 0.5f));  
  check (*zone, NodeTransformSpace_World, math::vec3f (0.0f, 0.0f, -1.0f), math::vec3f (0.0f, 0.0f, 2.0f));    
  check (*zone, NodeTransformSpace_World, math::vec3f (0.5f, 0.51f, -1.0f), math::vec3f (0.0f, 0.0f, 2.0f));    

  return 0;
}
