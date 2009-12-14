#include "shared.h"

int main ()
{
  printf ("Results of matrix06_test:\n");
  
  const float values1 [] = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f};
  
  mat4f m = values1;
  vec4f v1 (1.0f, 2.0f, 3.0f, 4.0f);
  vec3f v2 (1.0f, 2.0f, 3.0f);
  
  dump ("mat4f*vec4f", m*v1);
  dump ("vec4f*mat4f", v1*m);
  dump ("vec4f*=mat4f", vec4f (v1)*=m);
  dump ("mat4f*vec3f", m*v2);
  dump ("vec3f*mat4f", v2*m);
  dump ("vec3f*=mat4f", vec3f (v2)*=m);

  return 0;
}
