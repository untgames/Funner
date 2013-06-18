#include "shared.h"

struct A: public dynamic_cast_root
{
  virtual const char* name () const { return "class A"; }
};

struct B : A
{
  virtual const char* name () const { return "class B"; }
};

template class declcast<B*, A*, reinterpret_caster>;

int main ()
{
  printf ("Results of any_cast_speed_test:\n");

  B b;

  xtl::any a (&b);
  
  clock_t start = clock ();

  static const size_t N = 10000000;

  for (size_t i=0; i<N; i++)
    a.cast<A*> ();

  clock_t end = clock ();

  printf ("Casts per second: %.1fM\n", N / float (end-start) * CLOCKS_PER_SEC / 1000000);

  return 0;
}
