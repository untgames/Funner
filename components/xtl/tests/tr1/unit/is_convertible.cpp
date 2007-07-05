#include "test.h"

template <class T> struct convertible_from
{
  convertible_from (T);
};

struct base2 { };
struct middle2 : virtual base2 { };
struct derived2 : middle2 { };

int main ()
{
  printf ("Results of is_convertible_test:\n");

  CHECK_INTEGRAL_CONSTANT ((is_convertible<Derived,Base>::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_convertible<Derived,Derived>::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_convertible<Base,Base>::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_convertible<Base,Derived>::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_convertible<Derived,Derived>::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_convertible<NonDerived,Base>::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_convertible<float,int>::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_convertible<virtual_inherit2,virtual_inherit1>::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_convertible<VD,VB>::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_convertible<polymorphic_derived1,polymorphic_base>::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_convertible<polymorphic_derived2,polymorphic_base>::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_convertible<polymorphic_base,polymorphic_derived1>::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_convertible<polymorphic_base,polymorphic_derived2>::value), false);

  CHECK_INTEGRAL_CONSTANT ((is_convertible<test_abc1,test_abc1>::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_convertible<Base,test_abc1>::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_convertible<polymorphic_derived2,test_abc1>::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_convertible<int,test_abc1>::value), false);

  CHECK_INTEGRAL_CONSTANT ((is_convertible<float,void>::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_convertible<void,void>::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_convertible<void,float>::value), false);

  CHECK_INTEGRAL_CONSTANT ((is_convertible<enum1, int>::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_convertible<Derived*, Base*>::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_convertible<Base*, Derived*>::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_convertible<Derived&, Base&>::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_convertible<Base&, Derived&>::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_convertible<const Derived*, const Base*>::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_convertible<const Base*, const Derived*>::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_convertible<const Derived&, const Base&>::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_convertible<const Base&, const Derived&>::value), false);

  CHECK_INTEGRAL_CONSTANT ((is_convertible<const int *, int*>::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_convertible<const int&, int&>::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_convertible<const int*, int[3]>::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_convertible<const int&, int>::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_convertible<int(&)[4], const int*>::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_convertible<int(&)(int), int(*)(int)>::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_convertible<int *, const int*>::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_convertible<int&, const int&>::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_convertible<int[2], int*>::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_convertible<int[2], const int*>::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_convertible<const int[2], int*>::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_convertible<int*, int[3]>::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_convertible<test_abc3, const test_abc1&>::value), true);

  CHECK_INTEGRAL_CONSTANT ((is_convertible<non_pointer, void*>::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_convertible<non_pointer, int*>::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_convertible<non_int_pointer, int*>::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_convertible<non_int_pointer, void*>::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_convertible<test_abc1&, test_abc2&>::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_convertible<test_abc1&, int_constructible>::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_convertible<int_constructible, test_abc1&>::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_convertible<test_abc1&, test_abc2>::value), false);

  //
  // the following tests all involve user defined conversions which do
  // not compile with Borland C++ Builder 5:
  //
  CHECK_INTEGRAL_CONSTANT ((is_convertible<int, int_constructible>::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_convertible<float,convertible_from<float> >::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_convertible<float,convertible_from<float const&> >::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_convertible<float,convertible_from<float&> >::value), true);

  CHECK_INTEGRAL_CONSTANT ((is_convertible<float,convertible_from<char> >::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_convertible<float,convertible_from<char const&> >::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_convertible<float,convertible_from<char&> >::value), false);

  CHECK_INTEGRAL_CONSTANT ((is_convertible<char,convertible_from<char> >::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_convertible<char,convertible_from<char const&> >::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_convertible<char,convertible_from<char&> >::value), true);

  CHECK_INTEGRAL_CONSTANT ((is_convertible<float&,convertible_from<float> >::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_convertible<float const&,convertible_from<float> >::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_convertible<float&,convertible_from<float&> >::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_convertible<float const&,convertible_from<float const&> >::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_convertible<float&,convertible_from<float const&> >::value), true);

  CHECK_INTEGRAL_CONSTANT ((is_convertible<float,int>::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_convertible<double,int>::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_convertible<double,float>::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_convertible<long,int>::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_convertible<int,char>::value), true);
  
  return 0;
}
