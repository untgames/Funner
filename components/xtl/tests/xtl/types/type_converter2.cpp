#include <cstdio>

#include <xtl/custom_cast.h>

using namespace xtl;

class A {};
class B : public A {};

template class declcast<B, A>;

int main ()
{
  printf ("Results of type_converter2_test:\n");
  
  #define TEST(TO) printf ("check convertion B -> %s: %s\n", #TO, caster.has_cast<TO> () ? "true" : "false");

  B b;

  custom_ref_caster caster = make_custom_ref_caster (b);
  
  TEST (A&);
  TEST (A);
  TEST (A*);
  
  return 0;
}
