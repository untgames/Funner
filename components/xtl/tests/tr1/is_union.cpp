#include "test.h"

int main ()
{
   printf ("Results of is_union_test:\n");

   CHECK_INTEGRAL_CONSTANT (is_union<int>::value, false);
   CHECK_INTEGRAL_CONSTANT (is_union<const int>::value, false);
   CHECK_INTEGRAL_CONSTANT (is_union<volatile int>::value, false);
   CHECK_INTEGRAL_CONSTANT (is_union<int*>::value, false);
   CHECK_INTEGRAL_CONSTANT (is_union<int* const>::value, false);
   CHECK_INTEGRAL_CONSTANT (is_union<int[2]>::value, false);
   CHECK_INTEGRAL_CONSTANT (is_union<int&>::value, false);
   CHECK_INTEGRAL_CONSTANT (is_union<mf4>::value, false);
   CHECK_INTEGRAL_CONSTANT (is_union<f1>::value, false);
   CHECK_INTEGRAL_CONSTANT (is_union<enum_UDT>::value, false);

#ifdef MYTR1_HAS_TYPE_TRAITS_INTRINSICS
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

   CHECK_INTEGRAL_CONSTANT (is_union<UDT>::value, false);
   CHECK_INTEGRAL_CONSTANT (is_union<UDT const>::value, false);
   CHECK_INTEGRAL_CONSTANT (is_union<UDT volatile>::value, false);
   CHECK_INTEGRAL_CONSTANT (is_union<empty_UDT>::value, false);

   CHECK_INTEGRAL_CONSTANT (is_union<UDT*>::value, false);
   CHECK_INTEGRAL_CONSTANT (is_union<UDT[2]>::value, false);
   CHECK_INTEGRAL_CONSTANT (is_union<UDT&>::value, false);
   CHECK_INTEGRAL_CONSTANT (is_union<void>::value, false);

   CHECK_INTEGRAL_CONSTANT (is_union<test_abc1>::value, false);
   CHECK_INTEGRAL_CONSTANT (is_union<foo0_t>::value, false);
   CHECK_INTEGRAL_CONSTANT (is_union<foo1_t>::value, false);
   CHECK_INTEGRAL_CONSTANT (is_union<foo2_t>::value, false);
   CHECK_INTEGRAL_CONSTANT (is_union<foo3_t>::value, false);
   CHECK_INTEGRAL_CONSTANT (is_union<foo4_t>::value, false);
   CHECK_INTEGRAL_CONSTANT (is_union<incomplete_type>::value, false);

   return 0;
}
