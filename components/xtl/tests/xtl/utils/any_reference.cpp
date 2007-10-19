#include <stdio.h>
#include <xtl/any.h>

using namespace xtl;

struct A
{
  A ()         { printf ("A::A()\n"); }
  A (const A&) { printf ("A::A(const A&)\n"); }
  ~A ()        { printf ("A::~A()\n"); }
};

int main ()
{
  printf ("Results of any_reference_test:\n");
  
  any a = make_ref_any (A ()), b = a;

  return 0;
}
