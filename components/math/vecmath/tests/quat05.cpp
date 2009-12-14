#include "shared.h"

int main ()
{
  printf ("Results of quat05_test:\n");
  
  quatf q (1.0f, 2.0f, 3.0f, 4.0f);
  vec4f v (2.0f, 3.0f, 4.0f);
  
  dump ("q*v", q*v);
  dump ("v*q", v*q);
  dump ("q*vec3f(v)", q*vec3f (v));

  return 0;
}
