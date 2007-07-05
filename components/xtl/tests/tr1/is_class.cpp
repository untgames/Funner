#include "test.h"

int main ()
{
  printf ("Results of is_class_test:\n");

  CHECK_INTEGRAL_CONSTANT (is_class<int>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_class<const int>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_class<volatile int>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_class<int*>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_class<int* const>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_class<int[2]>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_class<int const[2]>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_class<int&>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_class<mf4>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_class<f1>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_class<enum_UDT>::value, false);

#ifdef MYTR1_HAS_TYPE_TRAITS_INTRINSICS
  CHECK_INTEGRAL_CONSTANT (is_class<union_UDT>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_class<POD_union_UDT>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_class<empty_union_UDT>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_class<empty_POD_union_UDT>::value, false);
#else
  printf ("This compiler version does not provide support for is_class on union types\n");
#endif

  CHECK_INTEGRAL_CONSTANT (is_class<UDT>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_class<UDT const>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_class<UDT volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_class<empty_UDT>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_class<test_abc1>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_class<test_abc1 const>::value, true);

  CHECK_INTEGRAL_CONSTANT (is_class<UDT*>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_class<UDT[2]>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_class<UDT&>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_class<void>::value, false);
  
  return 0;
}
