#include "test.h"

DECL_TRANSFORM_TEST(add_volatile_test_1, add_volatile, const, const volatile)
DECL_TRANSFORM_TEST(add_volatile_test_2, add_volatile, volatile, volatile)
DECL_TRANSFORM_TEST(add_volatile_test_3, add_volatile, *, *volatile)
DECL_TRANSFORM_TEST2(add_volatile_test_4, add_volatile, volatile)
DECL_TRANSFORM_TEST(add_volatile_test_7, add_volatile, *volatile, *volatile)
DECL_TRANSFORM_TEST(add_volatile_test_10, add_volatile, const*, const*volatile)
DECL_TRANSFORM_TEST(add_volatile_test_11, add_volatile, volatile*, volatile*volatile)
DECL_TRANSFORM_TEST(add_volatile_test_5, add_volatile, const &, const&)
DECL_TRANSFORM_TEST(add_volatile_test_6, add_volatile, &, &)
DECL_TRANSFORM_TEST(add_volatile_test_8, add_volatile, const [2], const volatile [2])
DECL_TRANSFORM_TEST(add_volatile_test_9, add_volatile, volatile &, volatile&)
DECL_TRANSFORM_TEST(add_volatile_test_12, add_volatile, [2][3], volatile[2][3])
DECL_TRANSFORM_TEST(add_volatile_test_13, add_volatile, (&)[2], (&)[2])

int main ()
{
  printf ("Results of add_volatile_test:\n");

  add_volatile_test_1();
  add_volatile_test_2();
  add_volatile_test_3();
  add_volatile_test_4();
  add_volatile_test_7();
  add_volatile_test_10();
  add_volatile_test_11();
  add_volatile_test_5();
  add_volatile_test_6();
  add_volatile_test_8();
  add_volatile_test_9();
  add_volatile_test_12();
  add_volatile_test_13();
  
  return 0;
}
