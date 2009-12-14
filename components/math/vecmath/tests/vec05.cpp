#include "shared.h"

const float EPS = 0.1f;

int main ()
{
  printf ("Results of vec05_test:\n");
  
  vec4f a (1.0f, 2.0f, 3.0f, 4.0f), b = a, c (b.x, b.y, b.z, b.w + EPS);
  
  dump ("a==b", a==b);
  dump ("a!=b", a!=b);
  dump ("a==c", a==c);
  dump ("a!=c", a!=c);
  dump ("equal1", equal (a, c, EPS));
  dump ("equal2", equal (a, c, EPS / 2));
    
  return 0;
}
