#include "test.h"

DECL_TRANSFORM_TEST(remove_extent_test_1, remove_extent, const, const)
DECL_TRANSFORM_TEST(remove_extent_test_2, remove_extent, volatile, volatile)
DECL_TRANSFORM_TEST3(remove_extent_test_3, remove_extent, [2])
DECL_TRANSFORM_TEST0(remove_extent_test_4, remove_extent)
DECL_TRANSFORM_TEST(remove_extent_test_5, remove_extent, const &, const&)
DECL_TRANSFORM_TEST(remove_extent_test_6, remove_extent, *, *)
DECL_TRANSFORM_TEST(remove_extent_test_7, remove_extent, *volatile, *volatile)
DECL_TRANSFORM_TEST(remove_extent_test_8, remove_extent, const [2], const)
DECL_TRANSFORM_TEST(remove_extent_test_9, remove_extent, const &, const&)
DECL_TRANSFORM_TEST(remove_extent_test_10, remove_extent, const*, const*)
DECL_TRANSFORM_TEST(remove_extent_test_11, remove_extent, volatile*, volatile*)
DECL_TRANSFORM_TEST(remove_extent_test_12, remove_extent, const[2][3], const[3])
DECL_TRANSFORM_TEST(remove_extent_test_13, remove_extent, (&)[2], (&)[2])
DECL_TRANSFORM_TEST3(remove_extent_test_14, remove_extent, [])
DECL_TRANSFORM_TEST(remove_extent_test_15, remove_extent, const [], const)
DECL_TRANSFORM_TEST(remove_extent_test_16, remove_extent, const[][3], const[3])

int main ()
{
  printf ("Results of remove_extent_test:\n");

  remove_extent_test_1();
  remove_extent_test_2();
  remove_extent_test_3();
  remove_extent_test_4();
  remove_extent_test_5();
  remove_extent_test_6();
  remove_extent_test_7();
  remove_extent_test_8();
  remove_extent_test_9();
  remove_extent_test_10();
  remove_extent_test_11();
  remove_extent_test_12();
  remove_extent_test_13();
  remove_extent_test_14();
  remove_extent_test_15();
  remove_extent_test_16();
   
  return 0;
}
