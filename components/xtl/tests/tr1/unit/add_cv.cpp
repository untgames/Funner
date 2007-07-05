#include "test.h"

DECL_TRANSFORM_TEST(add_cv_test_1, add_cv, const, const volatile)
DECL_TRANSFORM_TEST(add_cv_test_2, add_cv, volatile, volatile const)
DECL_TRANSFORM_TEST(add_cv_test_3, add_cv, *, *const volatile)
DECL_TRANSFORM_TEST2(add_cv_test_4, add_cv, const volatile)
DECL_TRANSFORM_TEST(add_cv_test_7, add_cv, *volatile, *volatile const)
DECL_TRANSFORM_TEST(add_cv_test_10, add_cv, const*, const*const volatile)
DECL_TRANSFORM_TEST(add_cv_test_11, add_cv, volatile*, volatile*const volatile )
DECL_TRANSFORM_TEST(add_cv_test_5, add_cv, const &, const&)
DECL_TRANSFORM_TEST(add_cv_test_6, add_cv, &, &)
DECL_TRANSFORM_TEST(add_cv_test_8, add_cv, const [2], const volatile [2])
DECL_TRANSFORM_TEST(add_cv_test_9, add_cv, volatile &, volatile&)
DECL_TRANSFORM_TEST(add_cv_test_12, add_cv, [2][3], const volatile[2][3])
DECL_TRANSFORM_TEST(add_cv_test_13, add_cv, (&)[2], (&)[2])

int main ()
{
  printf ("Results of add_cv_test:\n");

  add_cv_test_1();
  add_cv_test_2();
  add_cv_test_3();
  add_cv_test_4();
  add_cv_test_7();
  add_cv_test_10();
  add_cv_test_11();
  add_cv_test_5();
  add_cv_test_6();
  add_cv_test_8();
  add_cv_test_9();
  add_cv_test_12();
  add_cv_test_13();
  
  return 0;
}
