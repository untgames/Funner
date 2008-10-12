#include "test.h"

class polymorphic_no_virtual_destructor
{
  public:
    virtual ~polymorphic_no_virtual_destructor () {}

    virtual void method() = 0;
};

int main ()
{
  printf ("Results of has_virtual_destructor_test:\n");

  CHECK_INTEGRAL_CONSTANT (has_virtual_destructor<int>::value, false);
  CHECK_INTEGRAL_CONSTANT (has_virtual_destructor<const int>::value, false);
  CHECK_INTEGRAL_CONSTANT (has_virtual_destructor<volatile int>::value, false);
  CHECK_INTEGRAL_CONSTANT (has_virtual_destructor<int*>::value, false);
  CHECK_INTEGRAL_CONSTANT (has_virtual_destructor<int* const>::value, false);
  CHECK_INTEGRAL_CONSTANT (has_virtual_destructor<int[2]>::value, false);
  CHECK_INTEGRAL_CONSTANT (has_virtual_destructor<int&>::value, false);
  CHECK_INTEGRAL_CONSTANT (has_virtual_destructor<mf4>::value, false);
  CHECK_INTEGRAL_CONSTANT (has_virtual_destructor<f1>::value, false);
  CHECK_INTEGRAL_CONSTANT (has_virtual_destructor<enum_UDT>::value, false);

  CHECK_INTEGRAL_CONSTANT (has_virtual_destructor<UDT>::value, false);
  CHECK_INTEGRAL_CONSTANT (has_virtual_destructor<empty_UDT>::value, false);

  CHECK_INTEGRAL_CONSTANT (has_virtual_destructor<UDT*>::value, false);
  CHECK_INTEGRAL_CONSTANT (has_virtual_destructor<UDT[2]>::value, false);
  CHECK_INTEGRAL_CONSTANT (has_virtual_destructor<UDT&>::value, false);
  CHECK_INTEGRAL_CONSTANT (has_virtual_destructor<void>::value, false);

  CHECK_INTEGRAL_CONSTANT (has_virtual_destructor<polymorphic_no_virtual_destructor>::value, false);

  return 0;
}
