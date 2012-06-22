#include "shared.h"

void check (const planef& p, const vec3f& start, const vec3f& dir)
{
  float scale = 0.0f;
  
  if (!intersect (p, start, dir, scale))
  {
    printf ("no intersection\n");
  }
  else
  {
    math::vec3f s = start + dir * scale;
    
    printf ("intersection point: %.2f %.2f %.2f\n", s.x, s.y, s.z);
  }
}

int main ()
{
  printf ("Results of plane05_test:\n");
  
  check (planef (0.f, 0.f, 1.f, 10.f), math::vec3f (0.0f), math::vec3f (0.0f, 0.0f, 1.0f));
  check (planef (0.f, 0.f, 1.f, 10.f), math::vec3f (0.0f), math::vec3f (0.0f, 1.0f, 0.0f));
    
  return 0;
}
