#include "shared.h"

int main ()
{
  printf ("Results of vec03_test:\n");
  
  vec4f a (1.0f, 2.0f, 3.0f, 4.0f), b (5.0f, 6.0f, 7.0f, 8.0f);
  
  dump ("a+b", a+b);
  dump ("a-b", a-b);
  dump ("a*b", a*b);
  dump ("a/b", a/b);
  dump ("a*2", a*2.0f);
  dump ("a/2", a/2.0f);
  dump ("2*a", 2.0f*a);
  
  return 0;
}
