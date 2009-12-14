#include "shared.h"

int main ()
{
  printf ("Results of vec02_test:\n");
  
  vec4f a (1.0f, 2.0f, 3.0f, 4.0f), b (5.0f, 6.0f, 7.0f, 8.0f);
  
  vec4f c;
  
  c  = a;
  c += b;
  
  dump ("+=", c);
  
  c  = a;
  c -= b;
  
  dump ("-=", c);
  
  c  = a;
  c *= b;
  
  dump ("*=", c);

  c  = a;
  c /= b;
  
  dump ("/=", c);
  
  c  = a;
  c *= 2.0f;
  
  dump ("*= scalar", c);
  
  c  = a;
  c /= 2.0f;
  
  dump ("/= scalar", c);
  
  return 0;
}
