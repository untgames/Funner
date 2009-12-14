#include "shared.h"

int main ()
{
  printf ("Results of quat06_test:\n");
  
  quatf q (1.0f, 2.0f, 3.0f, 4.0f);

  dump ("q", q);
  dump ("norm(q)", norm (q));
  dump ("length(q)", length (q));
  dump ("normalize(q)", normalize (q));
  
  return 0;
}
