#include "test.h"

DECL_TRANSFORM_TEST(remove_reference_test_1, remove_reference, const, const)
DECL_TRANSFORM_TEST(remove_reference_test_2, remove_reference, volatile, volatile)
DECL_TRANSFORM_TEST3(remove_reference_test_3, remove_reference, &)
DECL_TRANSFORM_TEST0(remove_reference_test_4, remove_reference)
DECL_TRANSFORM_TEST(remove_reference_test_5, remove_reference, const &, const)
DECL_TRANSFORM_TEST(remove_reference_test_6, remove_reference, *, *)
DECL_TRANSFORM_TEST(remove_reference_test_7, remove_reference, *volatile, *volatile)
DECL_TRANSFORM_TEST3(remove_reference_test_8, remove_reference, &)
DECL_TRANSFORM_TEST(remove_reference_test_9, remove_reference, const &, const)
DECL_TRANSFORM_TEST(remove_reference_test_10, remove_reference, const*, const*)
DECL_TRANSFORM_TEST(remove_reference_test_11, remove_reference, volatile*, volatile*)
DECL_TRANSFORM_TEST(remove_reference_test_12, remove_reference, const[2], const[2])
DECL_TRANSFORM_TEST(remove_reference_test_13, remove_reference, (&)[2], [2])

int main ()
{
  printf ("Results of remove_reference_test:\n");

   remove_reference_test_1();
   remove_reference_test_2();
   remove_reference_test_3();
   remove_reference_test_4();
   remove_reference_test_5();
   remove_reference_test_6();
   remove_reference_test_7();
   remove_reference_test_8();
   remove_reference_test_9();
   remove_reference_test_10();
   remove_reference_test_11();
   remove_reference_test_12();
   remove_reference_test_13();

   return 0;
}
