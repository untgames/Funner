#include "shared.h"

int main ()
{
  printf ("Results of angle05_test:\n");
  
  anglef a;
  
  dump ("abs(degree(-30.0f))", abs (degree (-30.0f)));
  dump ("a", a);
  
  printf ("sin(a)=%.4f\n", sin (a));
  printf ("cos(a)=%.4f\n", cos (a));
  printf ("tan(a)=%.4f\n", tan (a));
  printf ("sinh(a)=%.4f\n", sinh (a));
  printf ("cosh(a)=%.4f\n", cosh (a));
  printf ("tanh(a)=%.4f\n", tanh (a));

  return 0;
}
