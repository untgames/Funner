#include "test.h"

DECL_TRANSFORM_TEST3(remove_cv_test_1, remove_cv, const)
DECL_TRANSFORM_TEST3(remove_cv_test_2, remove_cv, volatile)
DECL_TRANSFORM_TEST3(remove_cv_test_3, remove_cv, const volatile)
DECL_TRANSFORM_TEST0(remove_cv_test_4, remove_cv)
DECL_TRANSFORM_TEST(remove_cv_test_5, remove_cv, const &, const&)
DECL_TRANSFORM_TEST(remove_cv_test_6, remove_cv, *const, *)
DECL_TRANSFORM_TEST(remove_cv_test_7, remove_cv, *volatile, *)
DECL_TRANSFORM_TEST(remove_cv_test_8, remove_cv, *const volatile, *)
DECL_TRANSFORM_TEST(remove_cv_test_9, remove_cv, *, *)
DECL_TRANSFORM_TEST(remove_cv_test_10, remove_cv, const*, const*)
DECL_TRANSFORM_TEST(remove_cv_test_11, remove_cv, volatile*, volatile*)
DECL_TRANSFORM_TEST(remove_cv_test_12, remove_cv, const[2], [2])
DECL_TRANSFORM_TEST(remove_cv_test_13, remove_cv, volatile[2], [2])
DECL_TRANSFORM_TEST(remove_cv_test_14, remove_cv, const volatile[2], [2])
DECL_TRANSFORM_TEST(remove_cv_test_15, remove_cv, [2], [2])
DECL_TRANSFORM_TEST(remove_cv_test_16, remove_cv, const*, const*)
DECL_TRANSFORM_TEST(remove_cv_test_17, remove_cv, const*volatile, const*)
//DECL_TRANSFORM_TEST(remove_cv_test_18, remove_cv, volatile[], [])
//DECL_TRANSFORM_TEST(remove_cv_test_19, remove_cv, const volatile [], [])
//DECL_TRANSFORM_TEST(remove_cv_test_20, remove_cv, const [], [])
//DECL_TRANSFORM_TEST(remove_cv_test_21, remove_cv, [], [])

int main ()
{
  printf ("Results of remove_cv_test:\n");

  remove_cv_test_1();
  remove_cv_test_2();
  remove_cv_test_3();
  remove_cv_test_4();
  remove_cv_test_5();
  remove_cv_test_6();
  remove_cv_test_7();
  remove_cv_test_8();
  remove_cv_test_9();
  remove_cv_test_10();
  remove_cv_test_11();
  remove_cv_test_12();
  remove_cv_test_13();
  remove_cv_test_14();
  remove_cv_test_15();
  remove_cv_test_16();
  remove_cv_test_17();
//  remove_cv_test_18();
//  remove_cv_test_19();
//  remove_cv_test_20();
//  remove_cv_test_21();

  return 0;
}
