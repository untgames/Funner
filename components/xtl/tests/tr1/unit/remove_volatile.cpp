#include "test.h"

DECL_TRANSFORM_TEST(remove_volatile_test_1, remove_volatile, const, const)
DECL_TRANSFORM_TEST3(remove_volatile_test_2, remove_volatile, volatile)
DECL_TRANSFORM_TEST(remove_volatile_test_3, remove_volatile, const volatile, const)
DECL_TRANSFORM_TEST0(remove_volatile_test_4, remove_volatile)
DECL_TRANSFORM_TEST0(remove_volatile_test_5, remove_volatile)
DECL_TRANSFORM_TEST(remove_volatile_test_6, remove_volatile, *const, *const)
DECL_TRANSFORM_TEST(remove_volatile_test_7, remove_volatile, *volatile, *)
DECL_TRANSFORM_TEST(remove_volatile_test_8, remove_volatile, *const volatile, *const)
DECL_TRANSFORM_TEST(remove_volatile_test_9, remove_volatile, *, *)
DECL_TRANSFORM_TEST(remove_volatile_test_10, remove_volatile, *, *)
DECL_TRANSFORM_TEST(remove_volatile_test_11, remove_volatile, volatile*, volatile*)
DECL_TRANSFORM_TEST(remove_volatile_test_12, remove_volatile, const[2], const[2])
DECL_TRANSFORM_TEST(remove_volatile_test_13, remove_volatile, volatile[2], [2])
DECL_TRANSFORM_TEST(remove_volatile_test_14, remove_volatile, const volatile[2], const[2])
DECL_TRANSFORM_TEST(remove_volatile_test_15, remove_volatile, [2], [2])
DECL_TRANSFORM_TEST(remove_volatile_test_16, remove_volatile, const*, const*)
DECL_TRANSFORM_TEST(remove_volatile_test_17, remove_volatile, const*volatile, const*)
DECL_TRANSFORM_TEST(remove_volatile_test_18, remove_volatile, volatile[], [])
DECL_TRANSFORM_TEST(remove_volatile_test_19, remove_volatile, const volatile [], const [])
DECL_TRANSFORM_TEST(remove_volatile_test_20, remove_volatile, const [], const [])
DECL_TRANSFORM_TEST(remove_volatile_test_21, remove_volatile, [], [])

int main ()
{
  printf ("Results of remove_volatile_test:\n");

  remove_volatile_test_1();
  remove_volatile_test_2();
  remove_volatile_test_3();
  remove_volatile_test_4();
  remove_volatile_test_5();
  remove_volatile_test_6();
  remove_volatile_test_7();
  remove_volatile_test_8();
  remove_volatile_test_9();
  remove_volatile_test_10();
  remove_volatile_test_11();
  remove_volatile_test_12();
  remove_volatile_test_13();
  remove_volatile_test_14();
  remove_volatile_test_15();
  remove_volatile_test_16();
  remove_volatile_test_17();
  remove_volatile_test_18();
  remove_volatile_test_19();
  remove_volatile_test_20();
  remove_volatile_test_21();

  return 0;
}
