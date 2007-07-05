#include "test.h"

DECL_TRANSFORM_TEST(add_reference_test_1, add_reference, const, const&)
DECL_TRANSFORM_TEST(add_reference_test_2, add_reference, volatile, volatile&)
DECL_TRANSFORM_TEST(add_reference_test_3, add_reference, *, *&)
DECL_TRANSFORM_TEST2(add_reference_test_4, add_reference, &)
DECL_TRANSFORM_TEST(add_reference_test_5, add_reference, const &, const&)
DECL_TRANSFORM_TEST(add_reference_test_6, add_reference, &, &)
DECL_TRANSFORM_TEST(add_reference_test_7, add_reference, *volatile, *volatile&)
DECL_TRANSFORM_TEST(add_reference_test_8, add_reference, const [2], const (&)[2])
DECL_TRANSFORM_TEST(add_reference_test_9, add_reference, const &, const&)
DECL_TRANSFORM_TEST(add_reference_test_10, add_reference, const*, const*&)
DECL_TRANSFORM_TEST(add_reference_test_11, add_reference, volatile*, volatile*&)
DECL_TRANSFORM_TEST(add_reference_test_12, add_reference, const[2][3], const (&)[2][3])
DECL_TRANSFORM_TEST(add_reference_test_13, add_reference, (&)[2], (&)[2])

int main ()
{
  printf ("Results of add_reference_test:\n");

  add_reference_test_1();
  add_reference_test_2();
  add_reference_test_3();
  add_reference_test_4();
  add_reference_test_5();
  add_reference_test_6();
  add_reference_test_7();
  add_reference_test_8();
  add_reference_test_9();
  add_reference_test_10();
  add_reference_test_11();
  add_reference_test_12();
  add_reference_test_13();

  return 0;
}
