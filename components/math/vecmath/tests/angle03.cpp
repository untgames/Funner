#include "shared.h"

int main ()
{
  printf ("Results of angle03_test:\n");
  
  anglef a1 = degree (30.0f), a2 = degree (45.0f);

  dump ("a1", a1);
  dump ("a2", a2);  
  dump ("a1==a1", a1==a1);
  dump ("a1==a2", a1==a2);
  dump ("a1!=a2", a1!=a2);
  dump ("a1<a2", a1<a2);
  dump ("a1>a2", a1>a2);
  dump ("a1<=a2", a1<=a2);
  dump ("a1>=a2", a1>=a2);
  dump ("a1<=a1", a1<=a1);
  dump ("a1>=a1", a1>=a1);

  return 0;
}
