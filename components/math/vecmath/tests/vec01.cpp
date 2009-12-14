#include "shared.h"

int main ()
{
  printf ("Results of vec01_test:\n");
  
  vec4f v1;
  
  dump ("default constructor", v1);
  
  vec4f v2 = 1.0f;
  
  dump ("scalar constructor", v2);
  
  vec4f v3 (1.0f, 2.0f, 3.0f, 4.0f);
  
  dump ("vector constructor1", v3);

  vec4f v4 (1.0f, 2.0f, 3.0f);
  
  dump ("vector constructor2", v4);

  vec3f v5 (5.0f, 6.0f, 7.0f);
  vec4f v6 (v5, 8.0f);

  dump ("vector constructor3", v6);    
  
  vec3f v7 = v6;
  
  dump ("vector constructor4", v7);
  
  return 0;
}
