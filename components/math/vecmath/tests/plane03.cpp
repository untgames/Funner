#include "shared.h"

int main ()
{
  printf ("Results of plane03_test:\n");
  
  planef p (1.0f, 2.0f, 3.0f, 4.0f);
  
  dump ("normal (p)", p.normal ());
  p = normalize (p);
  dump ("normalize (p)", p);
  dump ("normal (p)", p.normal ());
    
  return 0;
}
