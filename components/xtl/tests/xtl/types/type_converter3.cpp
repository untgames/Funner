#include <cstdio>

#include <xtl/custom_cast.h>

using namespace xtl;

struct A { virtual ~A () {} };
struct B : public A {};
struct C : public A {};

namespace xtl
{

template class declcast<B, A>;
template class declcast<A, B, dynamic_caster>;
template class declcast<A, C, dynamic_caster>;

}

int main ()
{
  printf ("Results of type_converter3_test:\n");

  try
  {
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

    B* b1 = caster.cast<B*> ();

    printf ("b1 == &b: %s\n", b1 == &b ? "true" : "false");

    C* c = caster.cast<C*> ();

    printf ("c == &b: %s\n", static_cast<A*> (c) == &b ? "true" : "false");
  }
  catch (std::exception& e)
  {
    printf ("%s\n", e.what ());
  }
  catch (...)
  {
    printf ("unknown exception\n");
  }
  
  return 0;
}
