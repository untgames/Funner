#include "shared.h"

struct A
{
  A ()         { printf ("A::A()\n"); }
  A (const A&) { printf ("A::A(const A&)\n"); }
  ~A ()        { printf ("A::~A()\n"); }
};

int main ()
{
  printf ("Results of any_reference_test:\n");
  
  A var_a;
   
  any a = make_ref_any (make_const_ref (A ())), b = a, c = make_ref_any (make_const_ref (ref (var_a)));

  return 0;
}
