#ifndef __MYSTL_MATH_ADAPTORS__
#define __MYSTL_MATH_ADAPTORS__

#include <stl/type_traits.h>
#include <cmath>
#include <cstdlib>

namespace stl
{

/*
    Возведение в целую степень (время работы O(logN))
*/

template <class T,class Integer,class MonoidOperation> 
T power (T x,Integer n,MonoidOperation op);

template <class T,class Integer> 
T ipow (T x,Integer n);

/*
    Адаптеры вызовов математических функций
*/

namespace math_adaptors
{

template <class T> T abs   (T x);
template <class T> T acos  (T x);
template <class T> T asin  (T x);
template <class T> T atan  (T x);
template <class T> T atan2 (T x,T y);
template <class T> T ceil  (T x);
template <class T> T cos   (T x);
template <class T> T cosh  (T x);
template <class T> T exp   (T x);
template <class T> T fabs  (T x);
template <class T> T floor (T x);
template <class T> T fmod  (T x,T y);
template <class T> T frexp (T x,int* y);
template <class T> T ldexp (T x,int  y);
template <class T> T log   (T x);
template <class T> T log10 (T x);
template <class T> T modf  (T x,T* y);
template <class T> T sin   (T x);
template <class T> T sinh  (T x);
template <class T> T sqrt  (T x);
template <class T> T tan   (T x);
template <class T> T tanh  (T x);

template <class T,class T1> T pow (T x,T1 y);

}

#include <stl/detail/math.inl>

}

#endif
