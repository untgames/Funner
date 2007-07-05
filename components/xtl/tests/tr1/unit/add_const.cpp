#include "test.h"

DECL_TRANSFORM_TEST(add_const_test_1, add_const, const, const)
DECL_TRANSFORM_TEST(add_const_test_2, add_const, volatile, volatile const)
DECL_TRANSFORM_TEST(add_const_test_3, add_const, *, *const)
DECL_TRANSFORM_TEST2(add_const_test_4, add_const, const)
DECL_TRANSFORM_TEST(add_const_test_7, add_const, *volatile, *volatile const)
DECL_TRANSFORM_TEST(add_const_test_10, add_const, const*, const*const)
DECL_TRANSFORM_TEST(add_const_test_11, add_const, volatile*, volatile*const)
DECL_TRANSFORM_TEST(add_const_test_5, add_const, const &, const&)
DECL_TRANSFORM_TEST(add_const_test_6, add_const, &, &)
DECL_TRANSFORM_TEST(add_const_test_8, add_const, const [2], const [2])
DECL_TRANSFORM_TEST(add_const_test_9, add_const, volatile &, volatile&)
DECL_TRANSFORM_TEST(add_const_test_12, add_const, [2][3], const[2][3])
DECL_TRANSFORM_TEST(add_const_test_13, add_const, (&)[2], (&)[2])

int main ()
{
  printf ("Results of add_const_test:\n");

  add_const_test_1();
  add_const_test_2();
  add_const_test_3();
  add_const_test_4();
  add_const_test_7();
  add_const_test_10();
  add_const_test_11();
  add_const_test_5();
  add_const_test_6();
  add_const_test_8();
  add_const_test_9();
  add_const_test_12();
  add_const_test_13();
  
  return 0;
}
