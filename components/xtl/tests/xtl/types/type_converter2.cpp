#include <cstdio>

#include <xtl/custom_cast.h>

using namespace xtl;

class A {};
class B : public A {};

namespace xtl
{

template class declcast<B, A>;
template class declcast<A>;

}

int main ()
{
  printf ("Results of type_converter2_test:\n");
  
  #define TEST(TO) printf ("check convertion B -> %s: %s\n", #TO, caster.has_cast<TO> () ? "true" : "false");

  B b;

  custom_ref_caster caster = make_custom_ref_caster (b);
  
  TEST (A&);
  TEST (A);
  TEST (A*);
  TEST (B&);
  TEST (B);
  TEST (B*);

  A& a = caster.cast<A&> ();

  printf ("&a == &b: %s\n", &a == &b ? "true" : "false");

  caster = make_custom_ref_caster (a);

  #undef TEST
  #define TEST(TO) printf ("check convertion A -> %s: %s\n", #TO, caster.has_cast<TO> () ? "true" : "false");

  TEST (B&);
  TEST (B);
  TEST (B*);
  TEST (A);
  TEST (A&);
  TEST (A*);
  
  return 0;
}
