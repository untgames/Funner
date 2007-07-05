#include "test.h"

#ifdef _MSC_VER
  #pragma warning(disable:4244)
#endif

template <class T> struct align_calc
{
   char padding;
   T    instance;
   
   static ptrdiff_t get ()
   {
      static align_calc<T> a;
      
      return reinterpret_cast<const char*>(&(a.instance)) - reinterpret_cast<const char*>(&(a.padding));
   }
};

#define ALIGNOF(x) align_calc< x>::get()

int main ()
{
  printf ("Results of alignment_of_test:\n");

  CHECK_INTEGRAL_CONSTANT (alignment_of<void>::value, 0);
  CHECK_INTEGRAL_CONSTANT (alignment_of<char>::value, ALIGNOF(char));
  CHECK_INTEGRAL_CONSTANT (alignment_of<short>::value, ALIGNOF(short));
  CHECK_INTEGRAL_CONSTANT (alignment_of<int>::value, ALIGNOF(int));
  CHECK_INTEGRAL_CONSTANT (alignment_of<long>::value, ALIGNOF(long));
  CHECK_INTEGRAL_CONSTANT (alignment_of<float>::value, ALIGNOF(float));
  CHECK_INTEGRAL_CONSTANT (alignment_of<double>::value, ALIGNOF(double));
  CHECK_INTEGRAL_CONSTANT (alignment_of<long double>::value, ALIGNOF(long double));
  CHECK_INTEGRAL_CONSTANT (alignment_of<int[4]>::value, ALIGNOF(int[4]));
  CHECK_INTEGRAL_CONSTANT (alignment_of<int(*)(int)>::value, ALIGNOF(int(*)(int)));
  CHECK_INTEGRAL_CONSTANT (alignment_of<int*>::value, ALIGNOF(int*));
  CHECK_INTEGRAL_CONSTANT (alignment_of<VB>::value, ALIGNOF(VB));
  CHECK_INTEGRAL_CONSTANT (alignment_of<VD>::value, ALIGNOF(VD));
  CHECK_INTEGRAL_CONSTANT (alignment_of<enum_UDT>::value, ALIGNOF(enum_UDT));
  CHECK_INTEGRAL_CONSTANT (alignment_of<mf2>::value, ALIGNOF(mf2));
  CHECK_INTEGRAL_CONSTANT (alignment_of<POD_UDT>::value, ALIGNOF(POD_UDT));
  CHECK_INTEGRAL_CONSTANT (alignment_of<empty_UDT>::value, ALIGNOF(empty_UDT));
  CHECK_INTEGRAL_CONSTANT (alignment_of<union_UDT>::value, ALIGNOF(union_UDT));

  return 0;
}
