/*
    Начальный элемент для операций (0 для сложения, 1 для умножения)
*/

template <class T> inline T identity_element (plus<T>)
{
  return T (0);
}

template <class T> inline T identity_element (multiplies<T>)
{
  return T (1);
}

/*
    Возведение в целую степень
*/

template <class T,class Integer,class MonoidOperation>
T ipow (T x,Integer n,MonoidOperation op)
{
  if (n == 0)
    return identity_element (op);

  while ((n % 2) == 0) 
  {
    n /= 2;
    x  = op (x,x);
  }

  T result = x;

  while ((n/=2) != 0) 
  {
    x = op (x,x);

    if ((n % 2) != 0)
      result = op (result,x);
  }

  return result;
}

template <class T,class Integer>
inline T ipow (T x,Integer n)
{
  return ipow (x,n,multiplies<T>());
}

/*
    Адаптеры вызовов математических функций
*/

namespace math_adaptors
{

template <class T> inline T abs   (T x)        { return ::abs   (x); }
template <class T> inline T acos  (T x)        { return ::acos  (x); }
template <class T> inline T asin  (T x)        { return ::asin  (x); }
template <class T> inline T atan  (T x)        { return ::atan  (x); }
template <class T> inline T atan2 (T x,T y)    { return ::atan2 (x,y); }
template <class T> inline T ceil  (T x)        { return ::ceil  (x); }
template <class T> inline T cos   (T x)        { return ::cos   (x); }
template <class T> inline T cosh  (T x)        { return ::cosh  (x); }
template <class T> inline T exp   (T x)        { return ::exp   (x); }
template <class T> inline T fabs  (T x)        { return ::fabs  (x); }
template <class T> inline T floor (T x)        { return ::floor (x); }
template <class T> inline T fmod  (T x,T y)    { return ::fmod  (x,y); }
template <class T> inline T frexp (T x,int* y) { return ::frexp (x,y); }
template <class T> inline T ldexp (T x,int  y) { return ::ldexp (x,y); }
template <class T> inline T log   (T x)        { return ::log   (x); }
template <class T> inline T log10 (T x)        { return ::log10 (x); }
template <class T> inline T modf  (T x,T* y)   { return ::modf  (x,y); }
template <class T> inline T sin   (T x)        { return ::sin   (x); }
template <class T> inline T sinh  (T x)        { return ::sinh  (x); }
template <class T> inline T sqrt  (T x)        { return ::sqrt  (x); }
template <class T> inline T tan   (T x)        { return ::tan   (x); }
template <class T> inline T tanh  (T x)        { return ::tanh  (x); }

template <> inline float       abs (float  x)        { return fabsf (x); }
template <> inline double      abs (double x)        { return fabs  (x); }
template <> inline long double abs (long double x)   { return fabs (x); }
template <> inline int         abs (int x)           { return ::abs ((long)x); }
template <> inline short       abs (short x)         { return (short)::abs ((long)x); }
template <> inline signed char abs (signed char x)   { return (signed char)::abs ((long)x); }

template <class T,class T1> inline T pow (T x,T1 y)  { return ::pow (x,y); }
template <class T>          inline T pow (T x,int y) { return ipow  (x,y); }

}
