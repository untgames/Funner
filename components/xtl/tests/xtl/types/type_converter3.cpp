#include <cstdio>

#include <xtl/custom_cast.h>

using namespace xtl;

struct A { virtual ~A () {} };
struct B : public A {};
struct C : public A {};

template class declcast<B, A>;
template class declcast<A, B, dynamic_caster>;
template class declcast<B, C, dynamic_caster>;

int main ()
{
  printf ("Results of type_converter3_test:\n");

  B b;
  
  custom_ref_caster caster = make_custom_ref_caster<A> (b);
  
  #define TEST(TO) printf ("check convertion A -> %s: %s\n", #TO, caster.has_cast<TO> () ? "true" : "false");

  TEST (B&);
  TEST (B);
  TEST (B*);
  TEST (A);
  TEST (A&);
  TEST (A*);

  A* a = caster.cast<A*> ();

  printf ("a == &b: %s\n", a == &b ? "true" : "false");

  C* c = caster.cast<C*> ();

  printf ("c == &b: %s\n", a == &b ? "true" : "false");
  
  return 0;
}
