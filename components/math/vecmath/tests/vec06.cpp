#include "shared.h"

int main ()
{
  printf ("Results of vec06_test:\n");
  
  vec4f a (1.0f, 2.0f, 3.0f, 4.0f), b (2.0f, 3.0f, 4.0f, 5.0f);
  
  dump ("dot (a,b)", dot (a, b));
  dump ("cross (a,b)", cross (a, b));
    
  return 0;
}
