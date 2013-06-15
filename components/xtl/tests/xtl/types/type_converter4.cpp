#include <cstdio>

#include <xtl/custom_cast.h>

using namespace xtl;

struct A {};

int main ()
{
  printf ("Results of type_converter4_test:\n");

  try
  {
    A a;
    
    custom_ref_caster caster = make_custom_ref_caster (a);
    
    #define TEST(TO) printf ("check convertion A -> %s: %s\n", #TO, caster.has_cast<TO> () ? "true" : "false");

    TEST (A);
    TEST (A&);
    TEST (A*);
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
