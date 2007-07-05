#include "test.h"

int main ()
{
  printf ("Results of is_pod_test:\n");

  CHECK_INTEGRAL_CONSTANT (is_pod<bool>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_pod<bool const>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_pod<bool volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_pod<bool const volatile>::value, true);

  CHECK_INTEGRAL_CONSTANT (is_pod<signed char>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_pod<signed char const>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_pod<signed char volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_pod<signed char const volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_pod<unsigned char>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_pod<char>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_pod<unsigned char const>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_pod<char const>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_pod<unsigned char volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_pod<char volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_pod<unsigned char const volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_pod<char const volatile>::value, true);

  CHECK_INTEGRAL_CONSTANT (is_pod<unsigned short>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_pod<short>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_pod<unsigned short const>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_pod<short const>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_pod<unsigned short volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_pod<short volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_pod<unsigned short const volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_pod<short const volatile>::value, true);

  CHECK_INTEGRAL_CONSTANT (is_pod<unsigned int>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_pod<int>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_pod<unsigned int const>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_pod<int const>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_pod<unsigned int volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_pod<int volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_pod<unsigned int const volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_pod<int const volatile>::value, true);

  CHECK_INTEGRAL_CONSTANT (is_pod<unsigned long>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_pod<long>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_pod<unsigned long const>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_pod<long const>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_pod<unsigned long volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_pod<long volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_pod<unsigned long const volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_pod<long const volatile>::value, true);

  CHECK_INTEGRAL_CONSTANT (is_pod<float>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_pod<float const>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_pod<float volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_pod<float const volatile>::value, true);

  CHECK_INTEGRAL_CONSTANT (is_pod<double>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_pod<double const>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_pod<double volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_pod<double const volatile>::value, true);

  CHECK_INTEGRAL_CONSTANT (is_pod<long double>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_pod<long double const>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_pod<long double volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_pod<long double const volatile>::value, true);


  CHECK_INTEGRAL_CONSTANT (is_pod<int>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_pod<void*>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_pod<int*const>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_pod<f1>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_pod<f2>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_pod<f3>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_pod<mf1>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_pod<mf2>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_pod<mf3>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_pod<mp>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_pod<cmf>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_pod<enum_UDT>::value, true);

  CHECK_INTEGRAL_CONSTANT (is_pod<int&>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_pod<const int&>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_pod<int[2]>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_pod<int[3][2]>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_pod<int[2][4][5][6][3]>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_pod<UDT>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_pod<empty_UDT>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_pod<void>::value, true);

  CHECK_INTEGRAL_CONSTANT (is_pod<test_abc1>::value, false);

  CHECK_INTEGRAL_CONSTANT (is_pod<trivial_except_copy>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_pod<trivial_except_destroy>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_pod<trivial_except_assign>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_pod<trivial_except_construct>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_pod<wrap<trivial_except_copy> >::value, false);
  CHECK_INTEGRAL_CONSTANT (is_pod<wrap<trivial_except_assign> >::value, false);
  CHECK_INTEGRAL_CONSTANT (is_pod<wrap<trivial_except_destroy> >::value, false);
  CHECK_INTEGRAL_CONSTANT (is_pod<wrap<trivial_except_construct> >::value, false);

  return 0;
}
