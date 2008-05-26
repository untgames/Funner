#include "test.h"

int main ()
{
   printf ("Results of is_union_intrinsics_test:\n");

#ifdef XTL_HAS_TYPE_TRAITS_INTRINSICS
   CHECK_INTEGRAL_CONSTANT (is_union<union_UDT>::value, true);
   CHECK_INTEGRAL_CONSTANT (is_union<POD_union_UDT>::value, true);
   CHECK_INTEGRAL_CONSTANT (is_union<empty_union_UDT>::value, true);
   CHECK_INTEGRAL_CONSTANT (is_union<empty_POD_union_UDT>::value, true);
   CHECK_INTEGRAL_CONSTANT (is_union<union_UDT const>::value, true);
   CHECK_INTEGRAL_CONSTANT (is_union<POD_union_UDT volatile>::value, true);
   CHECK_INTEGRAL_CONSTANT (is_union<empty_union_UDT const volatile>::value, true);
   CHECK_INTEGRAL_CONSTANT (is_union<empty_POD_union_UDT const>::value, true);
#else
   printf ("This compiler version does not provide support for is_union on union types\n");
#endif

   return 0;
}
