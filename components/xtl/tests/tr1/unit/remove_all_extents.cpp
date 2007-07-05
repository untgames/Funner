#include "test.h"

DECL_TRANSFORM_TEST(remove_all_extents_test_1, remove_all_extents, const, const)
DECL_TRANSFORM_TEST(remove_all_extents_test_2, remove_all_extents, volatile, volatile)
DECL_TRANSFORM_TEST3(remove_all_extents_test_3, remove_all_extents, [2])
DECL_TRANSFORM_TEST0(remove_all_extents_test_4, remove_all_extents)
DECL_TRANSFORM_TEST(remove_all_extents_test_5, remove_all_extents, const &, const&)
DECL_TRANSFORM_TEST(remove_all_extents_test_6, remove_all_extents, *, *)
DECL_TRANSFORM_TEST(remove_all_extents_test_7, remove_all_extents, *volatile, *volatile)
DECL_TRANSFORM_TEST(remove_all_extents_test_8, remove_all_extents, const [2], const)
DECL_TRANSFORM_TEST(remove_all_extents_test_9, remove_all_extents, const &, const&)
DECL_TRANSFORM_TEST(remove_all_extents_test_10, remove_all_extents, const*, const*)
DECL_TRANSFORM_TEST(remove_all_extents_test_11, remove_all_extents, volatile*, volatile*)
DECL_TRANSFORM_TEST(remove_all_extents_test_12, remove_all_extents, const[2][3], const)
DECL_TRANSFORM_TEST(remove_all_extents_test_13, remove_all_extents, (&)[2], (&)[2])
DECL_TRANSFORM_TEST3(remove_all_extents_test_14, remove_all_extents, [])
DECL_TRANSFORM_TEST(remove_all_extents_test_15, remove_all_extents, const [], const)
DECL_TRANSFORM_TEST(remove_all_extents_test_16, remove_all_extents, const[][3], const)

int main ()
{
  printf ("Results of remove_all_extents_test:\n");

  remove_all_extents_test_1();
  remove_all_extents_test_2();
  remove_all_extents_test_3();
  remove_all_extents_test_4();
  remove_all_extents_test_5();
  remove_all_extents_test_6();
  remove_all_extents_test_7();
  remove_all_extents_test_8();
  remove_all_extents_test_9();
  remove_all_extents_test_10();
  remove_all_extents_test_11();
  remove_all_extents_test_12();
  remove_all_extents_test_13();
  remove_all_extents_test_14();
  remove_all_extents_test_15();
  remove_all_extents_test_16();

  return 0;
}
