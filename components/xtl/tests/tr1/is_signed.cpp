#include "test.h"

int main ()
{
  printf ("Resuts of is_signed_test:\n");

  CHECK_INTEGRAL_CONSTANT (is_signed<int>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_signed<long>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_signed<short>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_signed<signed char>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_signed<unsigned int>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_signed<unsigned long>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_signed<unsigned short>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_signed<unsigned char>::value, false);

  CHECK_INTEGRAL_CONSTANT (is_signed<UDT>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_signed<int&>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_signed<int*>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_signed<int[2]>::value, false);

  return 0;
}
