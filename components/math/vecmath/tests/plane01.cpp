#include "shared.h"

int main ()
{
  printf ("Results of plane01_test:\n");
  
  planei p1i;
  planef p1f;
  planed p1d;
  
  dump ("default int constructor", p1i);
  dump ("default float constructor", p1f);
  dump ("default double constructor", p1d);
  
  planef p2 (vec3f (1.f, 2.f, 3.f), 4.f);
  
  dump ("normal d constructor", p2);
  
  planef p3 (1.0f, 2.0f, 3.0f, 4.0f);
  
  dump ("constants constructor", p3);

  return 0;
}
