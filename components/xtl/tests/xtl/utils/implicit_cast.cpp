#include <xtl/implicit_cast.h>
#include <stdio.h>

using namespace xtl;

int   f (int x)   { return x; }
float f (float x) { return x; }

void test (int (*f)(int x))
{
  printf ("test<int>: %d\n", f (3));  
}

void test (float (*f)(float x))
{
  printf ("test<float>: %g\n", f (1.5f));
}

int main ()
{
  printf ("Results of implicit_cast_test:\n");
  
  test (implicit_cast<int (*)(int)> (&f));
  test (implicit_cast<float (*)(float)> (&f));

  return 0;
}
