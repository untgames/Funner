#include <cstdio>

#include <xtl/type_info.h>

using namespace xtl;

class A {};
class B : public A {};

mpl::list<A, B>::type get_cast_types (A*)
{
  return mpl::list<A, B>::type ();
}

int main ()
{
  printf ("Results of type_converter1_test:\n");
  
  #define TEST(FROM,TO) printf ("check convertion %s -> %s: %s\n", #FROM, #TO, has_custom_cast<TO, FROM> () ? "true" : "false");
  
  TEST (float, int);
  TEST (int, float);
  TEST (double, int);
  TEST (int, double);
  TEST (float, double);
  TEST (double, float);
  TEST (char, long double);
  TEST (long double, char);
  TEST (B&, A&);
  TEST (B, A);
  TEST (B*, A*);
  TEST (A, B);
  TEST (A&, B&);
  TEST (A*, B*);
  TEST (A, A);
  TEST (const A, A);
  TEST (volatile A, A);
  TEST (const volatile A, A);
  TEST (A, const A);
  TEST (A, volatile A);
  TEST (A, const volatile A);
  
  return 0;
}
