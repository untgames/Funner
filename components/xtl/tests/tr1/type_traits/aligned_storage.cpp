#include "test.h"

template <class T> union must_be_pod
{
   int i;
   T t;
};

template <class T> inline void no_unused_warning (const volatile T&) {}

template <class T>
void check (const T&)
{
   typedef typename aligned_storage<T::value, T::value>::type t1;
   
   t1 as1 = { 0, };
   
   must_be_pod<t1> pod1;
   
   no_unused_warning (as1);
   no_unused_warning (pod1);
   
   TEST (alignment_of<t1>::value == T::value);   
   TEST (sizeof(t1) == T::value);
   TEST (is_pod<t1>::value == true);

   typedef typename aligned_storage<T::value*2, T::value>::type t2;
   
   t2 as2 = { 0, };
   
   must_be_pod<t2> pod2;
   
   no_unused_warning (as2);
   no_unused_warning (pod2);
   
   TEST (alignment_of<t2>::value == T::value);
   TEST (sizeof(t2) == T::value*2);
   TEST (is_pod<t2>::value == true);

   typedef typename aligned_storage<T::value,-1L>::type t3;

   t3 as3 = { 0, };

   must_be_pod<t3> pod3;
   
   no_unused_warning (as3);
   no_unused_warning (pod3);

   TEST (alignment_of<t3>::value == alignment_of<xtl::type_traits::detail::max_align>::value);
   TEST ((sizeof(t3) % T::value) == 0);
   TEST (is_pod<t3>::value == true);
}

int main ()
{
  printf ("Results of aligned_storage_test:\n");

  check (integral_constant<size_t,alignment_of<char>::value>());
  check (integral_constant<size_t,alignment_of<short>::value>());
  check (integral_constant<size_t,alignment_of<int>::value>());
  check (integral_constant<size_t,alignment_of<long>::value>());
  check (integral_constant<size_t,alignment_of<float>::value>());
  check (integral_constant<size_t,alignment_of<double>::value>());
  check (integral_constant<size_t,alignment_of<long double>::value>());
  check (integral_constant<size_t,alignment_of<int[4]>::value>());
  check (integral_constant<size_t,alignment_of<int(*)(int)>::value>());
  check (integral_constant<size_t,alignment_of<int*>::value>());
  check (integral_constant<size_t,alignment_of<VB>::value>());
  check (integral_constant<size_t,alignment_of<VD>::value>());
  check (integral_constant<size_t,alignment_of<enum_UDT>::value>());
  check (integral_constant<size_t,alignment_of<mf2>::value>());
  check (integral_constant<size_t,alignment_of<POD_UDT>::value>());
  check (integral_constant<size_t,alignment_of<empty_UDT>::value>());
  check (integral_constant<size_t,alignment_of<union_UDT>::value>());
  check (integral_constant<size_t,alignment_of<xtl::type_traits::detail::max_align>::value>());

  return 0;
}
