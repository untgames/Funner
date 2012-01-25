#include <cstdio>
#include <limits.h>

#include <xtl/rational.h>

using namespace xtl;

#define TEST(X) printf ((X)?"expression '%s' is true\n":"failed expression '%s'\n",#X);

int main ()
{
  printf ("Resultss of rational_example_test:\n");

  rational<int> half (1,2);
  rational<int> one (1);
  rational<int> two (2);

  // Some basic checks
  TEST (half.numerator() == 1);
  TEST (half.denominator() == 2);
  TEST (xtl::rational_cast<double> (half) == 0.5);

  // Arithmetic
  TEST (half + half == one);
  TEST (one - half == half);
  TEST (two * half == one);
  TEST (one / half == two);

  // With conversions to integer
  TEST (half + half == 1);
  TEST (2 * half == one);
  TEST (2 * half == 1);
  TEST (one / half == 2);
  TEST (1 / half == 2);

  // Sign handling
  rational<int> minus_half(-1,2);
  TEST (-half == minus_half);
  TEST (abs(minus_half) == half);

  // Do we avoid overflow?
  int maxint = INT_MAX;

  rational<int> big(maxint, 2);
  TEST (2 * big == maxint);

  // Exception handling
  try
  {
      rational<int> r;        // Forgot to initialise - set to 0
      r = 1/r;                // Boom!
  }
  catch (const xtl::bad_rational &e)
  {
    printf ("Bad rational, as expected: %s\n", e.what());
  }
  catch (...) {
    printf ("Wrong exception raised!\n");
  }

  return 0;
}
