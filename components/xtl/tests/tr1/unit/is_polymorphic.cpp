#include "test.h"

#include <exception>
#include <new>
#include <stdexcept>

struct poly_bug { virtual int foo() = 0; };

int main ()
{
  printf ("Results of is_polymorphic_test:\n");

  CHECK_INTEGRAL_CONSTANT (is_polymorphic<int>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_polymorphic<const int>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_polymorphic<volatile int>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_polymorphic<int*>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_polymorphic<int* const>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_polymorphic<int[2]>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_polymorphic<int&>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_polymorphic<mf4>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_polymorphic<f1>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_polymorphic<enum_UDT>::value, false);

  CHECK_INTEGRAL_CONSTANT (is_polymorphic<UDT>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_polymorphic<empty_UDT>::value, false);

  CHECK_INTEGRAL_CONSTANT (is_polymorphic<UDT*>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_polymorphic<UDT[2]>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_polymorphic<UDT&>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_polymorphic<void>::value, false);

  CHECK_INTEGRAL_CONSTANT (is_polymorphic<VB>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_polymorphic<VD>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_polymorphic<test_abc1>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_polymorphic<test_abc2>::value, true);

  CHECK_INTEGRAL_CONSTANT (is_polymorphic<std::exception>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_polymorphic<std::bad_alloc>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_polymorphic<std::runtime_error>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_polymorphic<std::out_of_range>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_polymorphic<std::range_error>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_polymorphic<poly_bug>::value, true);
  
  return 0;
}
