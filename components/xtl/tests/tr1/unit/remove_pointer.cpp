#include "test.h"

DECL_TRANSFORM_TEST(remove_pointer_test_1, remove_pointer, const, const)
DECL_TRANSFORM_TEST(remove_pointer_test_2, remove_pointer, volatile, volatile)
DECL_TRANSFORM_TEST3(remove_pointer_test_3, remove_pointer, *)
DECL_TRANSFORM_TEST0(remove_pointer_test_4, remove_pointer)
DECL_TRANSFORM_TEST(remove_pointer_test_5, remove_pointer, const &, const&)
DECL_TRANSFORM_TEST(remove_pointer_test_6, remove_pointer, &, &)
DECL_TRANSFORM_TEST3(remove_pointer_test_7, remove_pointer, *volatile)
DECL_TRANSFORM_TEST(remove_pointer_test_8, remove_pointer, const [2], const[2])
DECL_TRANSFORM_TEST(remove_pointer_test_9, remove_pointer, const &, const&)
DECL_TRANSFORM_TEST(remove_pointer_test_10, remove_pointer, const*, const)
DECL_TRANSFORM_TEST(remove_pointer_test_11, remove_pointer, volatile*, volatile)
DECL_TRANSFORM_TEST(remove_pointer_test_12, remove_pointer, const[2][3], const[2][3])
DECL_TRANSFORM_TEST(remove_pointer_test_13, remove_pointer, (&)[2], (&)[2])

int main ()
{
  printf ("Results of remove_pointer_test:\n");

  remove_pointer_test_1();
  remove_pointer_test_2();
  remove_pointer_test_3();
  remove_pointer_test_4();
  remove_pointer_test_5();
  remove_pointer_test_6();
  remove_pointer_test_7();
  remove_pointer_test_8();
  remove_pointer_test_9();
  remove_pointer_test_10();
  remove_pointer_test_11();
  remove_pointer_test_12();
  remove_pointer_test_13();
  
  return 0;
}
