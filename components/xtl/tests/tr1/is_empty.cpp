#include "test.h"

struct non_default_constructable_UDT
{
  non_default_constructable_UDT(const non_default_constructable_UDT&){}
  
  private:
  non_default_constructable_UDT(){}
};

int main ()
{
  printf ("Results of is_empty_test:\n");

  CHECK_INTEGRAL_CONSTANT (is_empty<int>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_empty<int*>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_empty<int&>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_empty<void>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_empty<int[2]>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_empty<f1>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_empty<mf1>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_empty<UDT>::value, false);

//  BOOST_CHECK_SOFT_INTEGRAL_CONSTANT(::tt::is_empty<empty_UDT>::value, true, false);
//  BOOST_CHECK_SOFT_INTEGRAL_CONSTANT(::tt::is_empty<empty_POD_UDT>::value, true, false);
//  BOOST_CHECK_SOFT_INTEGRAL_CONSTANT(::tt::is_empty<non_default_constructable_UDT>::value, true, false);
//  BOOST_CHECK_SOFT_INTEGRAL_CONSTANT(::tt::is_empty<boost::noncopyable>::value, true, false);

  CHECK_INTEGRAL_CONSTANT (is_empty<enum_UDT>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_empty<non_empty>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_empty<const non_empty&>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_empty<foo4_t>::value, false);

  return 0;
}
