#include "test.h"

int main ()
{
  printf ("Results of is_base_of_test:\n");

  CHECK_INTEGRAL_CONSTANT ((is_base_of<Derived,Base>::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_base_of<Derived,Derived>::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_base_of<Base,Base>::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_base_of<Base,Derived>::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_base_of<Base,MultiBase>::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_base_of<Derived,MultiBase>::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_base_of<Derived2,MultiBase>::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_base_of<Base,PrivateBase>::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_base_of<NonDerived,Base>::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_base_of<Base,void>::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_base_of<Base,const void>::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_base_of<void,Derived>::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_base_of<const void,Derived>::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_base_of<int, int>::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_base_of<const int, int>::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_base_of<VB,VD>::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_base_of<VD,VB>::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_base_of<test_abc1,test_abc3>::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_base_of<test_abc3,test_abc1>::value), false);

  return 0;
}
