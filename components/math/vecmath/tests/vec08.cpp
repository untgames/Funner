#include "shared.h"

int main ()
{
  printf ("Results of vec08_test:\n");
  
  vec4f a (1.0f, 2.0f, 3.0f, 4.0f), b (-7.0f, -8.0f, 6.0f, 5.0f);
  
  dump ("min (a,b)", min (a, b));
  dump ("max (a,b)", max (a, b));
  dump ("abs (b)", abs (b));
  
  return 0;
}
