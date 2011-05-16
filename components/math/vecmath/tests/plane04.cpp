#include "shared.h"

const char* side_name (plane_side side)
{
  switch (side)
  {
    case plane_side_none:     return "none";
    case plane_side_positive: return "positive";
    case plane_side_negative: return "negative";
    case plane_side_both:     return "both";
    default:                  return "unknown";
  }
}

int main ()
{
  printf ("Results of plane04_test:\n");
  
  planef p (0.f, 0.f, 1.f, 10.f);

  printf ("distance 1 = %f\n", distance (p, vec3f (10.f, 15.f, 25.f)));
  printf ("distance 2 = %f\n", distance (p, vec3f (10.f, 15.f, -25.f)));
  dump ("project 1", project (p, vec3f (10.f, 0.f, 0.f)));
  dump ("project 2", project (p, vec3f (10.f, 10.f, 10.f)));
  printf ("side 1 = %s\n", side_name (side (p, vec3f (10.f, 15.f, 20.f))));
  printf ("side 2 = %s\n", side_name (side (p, vec3f (10.f, 15.f, -20.f))));
  printf ("side 3 = %s\n", side_name (side (p, vec3f (10.f, 15.f, 10.f))));
  printf ("side 4 = %s\n", side_name (side (p, vec3f (10.f, 15.f, -10.1f), 0.101f)));
  printf ("side 5 = %s\n", side_name (side (p, vec3f (10.f, 15.f, -10.1f), 0.05f)));
    
  return 0;
}
