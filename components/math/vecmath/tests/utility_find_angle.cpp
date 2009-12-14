#include "shared.h"

int main ()
{
  printf ("Results of utility_find_angle_test:\n");
  
  vec4f a (1.0f, 2.0f, 3.0f, 4.0f), b (3.0f, 4.0f, 5.0f);
  
  dump ("find_angle (a,b)", find_angle (a, b));
  
  return 0;
}
