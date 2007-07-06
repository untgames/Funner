#include "test.h"

int main ()
{
  printf ("Results of has_trivial_destructor_test:\n");

  CHECK_INTEGRAL_CONSTANT (has_trivial_destructor<bool>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_destructor<bool const>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_destructor<bool volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_destructor<bool const volatile>::value, true);

  CHECK_INTEGRAL_CONSTANT (has_trivial_destructor<signed char>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_destructor<signed char const>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_destructor<signed char volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_destructor<signed char const volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_destructor<unsigned char>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_destructor<char>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_destructor<unsigned char const>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_destructor<char const>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_destructor<unsigned char volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_destructor<char volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_destructor<unsigned char const volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_destructor<char const volatile>::value, true);

  CHECK_INTEGRAL_CONSTANT (has_trivial_destructor<unsigned short>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_destructor<short>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_destructor<unsigned short const>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_destructor<short const>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_destructor<unsigned short volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_destructor<short volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_destructor<unsigned short const volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_destructor<short const volatile>::value, true);

  CHECK_INTEGRAL_CONSTANT (has_trivial_destructor<unsigned int>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_destructor<int>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_destructor<unsigned int const>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_destructor<int const>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_destructor<unsigned int volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_destructor<int volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_destructor<unsigned int const volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_destructor<int const volatile>::value, true);

  CHECK_INTEGRAL_CONSTANT (has_trivial_destructor<unsigned long>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_destructor<long>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_destructor<unsigned long const>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_destructor<long const>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_destructor<unsigned long volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_destructor<long volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_destructor<unsigned long const volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_destructor<long const volatile>::value, true);

  CHECK_INTEGRAL_CONSTANT (has_trivial_destructor<float>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_destructor<float const>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_destructor<float volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_destructor<float const volatile>::value, true);

  CHECK_INTEGRAL_CONSTANT (has_trivial_destructor<double>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_destructor<double const>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_destructor<double volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_destructor<double const volatile>::value, true);

  CHECK_INTEGRAL_CONSTANT (has_trivial_destructor<long double>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_destructor<long double const>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_destructor<long double volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_destructor<long double const volatile>::value, true);

  CHECK_INTEGRAL_CONSTANT (has_trivial_destructor<int>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_destructor<void*>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_destructor<int*const>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_destructor<f1>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_destructor<f2>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_destructor<f3>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_destructor<mf1>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_destructor<mf2>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_destructor<mf3>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_destructor<mp>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_destructor<cmf>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_destructor<enum_UDT>::value, true);

  CHECK_INTEGRAL_CONSTANT (has_trivial_destructor<int[2]>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_destructor<int[3][2]>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_destructor<int[2][4][5][6][3]>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_destructor<UDT>::value, false);
  CHECK_INTEGRAL_CONSTANT (has_trivial_destructor<empty_UDT>::value, false);
  CHECK_INTEGRAL_CONSTANT (has_trivial_destructor<void>::value, true);

  CHECK_INTEGRAL_CONSTANT (has_trivial_destructor<trivial_except_destroy>::value, false);
  CHECK_INTEGRAL_CONSTANT (has_trivial_destructor<wrap<trivial_except_destroy> >::value, false);
  
  return 0;
}
