#include "shared.h"

int main ()
{
  printf ("Results of angle01_test:\n");
  
  anglef a1;
  
  dump ("default constructor", a1);
  
  dump ("degree constructor", degree (30.0f));
  dump ("radian constructor", radian (constf::pi / 2.0f));
  dump ("convert constructor", anglef (degree (60.0)));
  dump ("copy constructor", anglef (degree (45.0f)));
  
  a1 = degree (10.0f);
  
  dump ("assignment degree", a1);
  
  a1 = radian (constf::pi / 3.0f);
  
  dump ("assignment radian", a1);
  
  a1 = radian (constd::pi / 4.0f);
  
  dump ("assignment with convert", a1);
  
  a1 = anglef ();
  
  dump ("assignment", a1);

  return 0;
}
