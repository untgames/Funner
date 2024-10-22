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
