#include "shared.h"

int main ()
{
  printf ("Results of angle04_test:\n");
  
  anglef a = degree (45.0f);
  
  dump ("a", a);
  printf ("a.to_radian (): %.4f\n", a.to_radian ());
  printf ("a.to_degree (): %.4f\n", a.to_degree ());
  printf ("a.get (radian_tag ()): %.4f\n", a.get (radian_tag ()));
  printf ("a.get (degree_tag ()): %.4f\n", a.get (degree_tag ()));
  printf ("radian (a): %.4f\n", radian (a));
  printf ("degree (a): %.4f\n", degree (a));

  return 0;
}
