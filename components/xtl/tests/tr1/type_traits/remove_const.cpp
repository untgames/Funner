#include "test.h"

DECL_TRANSFORM_TEST3(remove_const_test_1, remove_const, const)
DECL_TRANSFORM_TEST(remove_const_test_2, remove_const, volatile, volatile)
DECL_TRANSFORM_TEST(remove_const_test_3, remove_const, const volatile, volatile)
DECL_TRANSFORM_TEST0(remove_const_test_4, remove_const)
DECL_TRANSFORM_TEST(remove_const_test_6, remove_const, *const, *)
DECL_TRANSFORM_TEST(remove_const_test_7, remove_const, *volatile, *volatile)
DECL_TRANSFORM_TEST(remove_const_test_8, remove_const, *const volatile, *volatile)
DECL_TRANSFORM_TEST(remove_const_test_9, remove_const, *, *)
DECL_TRANSFORM_TEST(remove_const_test_11, remove_const, volatile*, volatile*)
DECL_TRANSFORM_TEST(remove_const_test_12, remove_const, const[2], [2])
DECL_TRANSFORM_TEST(remove_const_test_13, remove_const, volatile[2], volatile[2])
DECL_TRANSFORM_TEST(remove_const_test_14, remove_const, const volatile[2], volatile[2])
DECL_TRANSFORM_TEST(remove_const_test_15, remove_const, [2], [2])
DECL_TRANSFORM_TEST(remove_const_test_16, remove_const, const*, const*)
DECL_TRANSFORM_TEST(remove_const_test_17, remove_const, const*const, const*)
/*DECL_TRANSFORM_TEST(remove_const_test_18, remove_const, const[], [])
DECL_TRANSFORM_TEST(remove_const_test_19, remove_const, const volatile [], volatile [])
DECL_TRANSFORM_TEST(remove_const_test_20, remove_const, volatile [], volatile [])
DECL_TRANSFORM_TEST(remove_const_test_21, remove_const, [], [])*/

int main ()
{
  printf ("Results of remove_const_test:\n");

  remove_const_test_1();
  remove_const_test_2();
  remove_const_test_3();
  remove_const_test_4();
  remove_const_test_6();
  remove_const_test_7();
  remove_const_test_8();
  remove_const_test_9();
  remove_const_test_11();
  remove_const_test_12();
  remove_const_test_13();
  remove_const_test_14();
  remove_const_test_15();
  remove_const_test_16();
  remove_const_test_17();
//  remove_const_test_18();
//  remove_const_test_19();
//  remove_const_test_20();
//  remove_const_test_21();
  
  return 0;
}
