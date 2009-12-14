#include "shared.h"

int main ()
{
  printf ("Results of quat08_test:\n");
  
  quatf q (1.0f, 2.0f, 3.0f, 4.0f);

  dump ("q", q);
  dump ("inverse (q)", inverse (q));
  dump ("test inverse", equal (quatf (), inverse (q) * q, 0.1f));
  
  return 0;
}
