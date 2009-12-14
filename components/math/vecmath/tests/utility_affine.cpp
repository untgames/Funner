#include "shared.h"

int main ()
{
  printf ("Results of utility_affine_test:\n");
  
  dump ("translate", translate (vec3f (1, 2, 3)));
  dump ("rotate", rotate (degree (90.0f), vec3f (0, 0, 1)));
  dump ("scale", scale (vec3f (1, 2, 3)));
  dump ("lookat", lookat (vec3f (1, 0, 0), vec3f (0), vec3f (0, 1, 0)));

  return 0;
}
