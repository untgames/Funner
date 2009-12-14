#include "shared.h"

int main ()
{
  printf ("Results of vec04_test:\n");
  
  vec4f a (1.0f, 2.0f, 3.0f, 4.0f), b;
  
  b = a;
  
  dump ("assign vector4", b);
  
  b = vec3f (10.0f, 20.0f, 30.0f);
  
  dump ("assign vector3", b);
  
  b = 1.0f;
  
  dump ("assign scalar", b);
  
  dump ("-a", -a);
  dump ("+a", +a);
  
  return 0;
}
