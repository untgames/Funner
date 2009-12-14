#include "shared.h"

int main ()
{
  printf ("Results of vec07_test:\n");
  
  vec4f a (1.0f, 2.0f, 3.0f, 4.0f);
  
  dump ("length (a)", length (a));
  dump ("qlen (a)", qlen (a));
  dump ("normalize (a)", normalize (a));
    
  return 0;
}
