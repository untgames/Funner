#include "shared.h"

int main ()
{
  printf ("Results of angle02_test:\n");
  
  anglef a1 = degree (30.0f), a2 = degree (45.0f);
  
  dump ("a1", a1);
  dump ("a2", a2);
  dump ("+a1", +a1);
  dump ("-a1", -a1);
  dump ("a1+a2", a1+a2);
  dump ("a1-a2", a1-a2);
  dump ("a1*2", a1*2.0f);
  dump ("2*a1", 2.0f*a1);
  dump ("a1/2", a1/2);
  dump ("a1+=a2", anglef (a1)+=a2);
  dump ("a1-=a2", anglef (a1)-=a2);
  dump ("a1*=2", anglef (a1)*=2.0f);
  dump ("a1/=2", anglef (a1)/=2);

  return 0;
}
