#ifndef __MYSTL_PAIR__
#define __MYSTL_PAIR__

#include <stl/config.h>

#ifdef _MSC_VER 
  #pragma warning (disable:4512) //'class' : assignment operator could not be generated
#endif

namespace stl
{

////////////////////////////////////////////////////////////////////
///Пара значений
////////////////////////////////////////////////////////////////////
template <class T1,class T2>
struct pair 
{
  typedef T1 first_type;
  typedef T2 second_type;

  T1 first;
  T2 second;
  
  pair  () {}
  pair  (const T1& a,const T2& b);
  ~pair () {}

  template <class U1,class U2> pair (const pair<U1,U2>&);
  
////////////////////////////////////////////////////////////////////
///Операции сравнения
////////////////////////////////////////////////////////////////////  
  bool operator == (const pair&) const;
  bool operator != (const pair&) const;
  bool operator <  (const pair&) const;
  bool operator >  (const pair&) const;
  bool operator <= (const pair&) const;
  bool operator >= (const pair&) const;
};

////////////////////////////////////////////////////////////////////
///Создание пары
////////////////////////////////////////////////////////////////////  
template <class T1,class T2>
pair<T1,T2> make_pair (const T1& x,const T2& y);

#include <stl/detail/pair.inl>

#ifdef _MSC_VER 
  #pragma warning (default:4512) //'class' : assignment operator could not be generated
#endif

}

#endif
