#include "test.h"

int main ()
{
  printf ("Results of is_unsigned_test:\n");

  CHECK_INTEGRAL_CONSTANT (is_unsigned<int>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_unsigned<long>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_unsigned<short>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_unsigned<signed char>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_unsigned<unsigned int>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_unsigned<unsigned long>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_unsigned<unsigned short>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_unsigned<unsigned char>::value, true);

  CHECK_INTEGRAL_CONSTANT (is_unsigned<UDT>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_unsigned<int&>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_unsigned<int*>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_unsigned<int[2]>::value, false);

  return 0;
}
