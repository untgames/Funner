#include "test.h"

int main ()
{
  printf ("Results of is_class_intrinsics_test:\n");

#ifdef XTL_HAS_TYPE_TRAITS_INTRINSICS
  CHECK_INTEGRAL_CONSTANT (is_class<union_UDT>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_class<POD_union_UDT>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_class<empty_union_UDT>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_class<empty_POD_union_UDT>::value, false);
#else
  printf ("This compiler version does not provide support for is_class on union types\n");
#endif

  return 0;
}
