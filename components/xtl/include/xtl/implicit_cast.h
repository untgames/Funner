#ifndef XTL_IMPLICIT_CAST_HEADER
#define XTL_IMPLICIT_CAST_HEADER

namespace xtl
{

//аргумент явного приведения типов
template <class T> struct implicit_cast_argument { typedef T type; };

//функция-обёртка для явного приведения типов
template <class T> inline T implicit_cast (typename implicit_cast_argument<T>::type x)
{
  return x;
}

}

#endif
