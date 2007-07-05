#include "test.h"

DECL_TRANSFORM_TEST(add_pointer_test_1, add_pointer, const, const*)
DECL_TRANSFORM_TEST(add_pointer_test_2, add_pointer, volatile, volatile*)
DECL_TRANSFORM_TEST(add_pointer_test_3, add_pointer, *, **)
DECL_TRANSFORM_TEST2(add_pointer_test_4, add_pointer, *)
DECL_TRANSFORM_TEST(add_pointer_test_7, add_pointer, *volatile, *volatile*)
DECL_TRANSFORM_TEST(add_pointer_test_10, add_pointer, const*, const**)
DECL_TRANSFORM_TEST(add_pointer_test_11, add_pointer, volatile*, volatile**)

int main ()
{
  printf ("Results of add_pointer_test:\n");

  add_pointer_test_1();
  add_pointer_test_2();
  add_pointer_test_3();
  add_pointer_test_4();
  add_pointer_test_7();
  add_pointer_test_10();
  add_pointer_test_11();
  
  return 0;
}
