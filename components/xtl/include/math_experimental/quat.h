#ifndef MATHLIB_QUATERNION_HEADER
#define MATHLIB_QUATERNION_HEADER

#include <math_experimental/forward.h>
#include "functional.h"
#include <stddef.h>
#ifdef _MSC_VER
  #pragma pack (push,1)
#endif

namespace math
{


template <class type> struct quat_base
{
  type x,y,z,w;

  type&       operator [] (size_t index)       { return (&x) [index]; }
  const type& operator [] (size_t index) const { return (&x) [index]; }
};

/////////////////////////////////////////////////////////////////////////////////////////////
///Кватернион
/////////////////////////////////////////////////////////////////////////////////////////////
template <class type> 
class quat: public quat_base<type>
{
  public:
    typedef type value_type;
    enum {size=4};
  
    explicit quat ();
    explicit quat (const type& x_,const type& y_,const type& z_,const type& w_);
    explicit quat (const type& a);
       //для использования оптимизации возвращаемого значения

    template <class T,class Fn>           quat (const T&,Fn fn);
    template <class T1,class T2,class Fn> quat (const T1&,const T2&,Fn fn);

////////////////////////////////////////////////////////////////////////////////////////////
///Унарные операции
////////////////////////////////////////////////////////////////////////////////////////////
    const quat& operator +              () const { return *this; }
    const quat  operator -              () const;
    type        length                  () const;
    type        squared_length          () const;

////////////////////////////////////////////////////////////////////////////////////////////
///Присваивание
////////////////////////////////////////////////////////////////////////////////////////////
    quat&   operator = (const type& a);  
    quat&   operator = (const quat_base<type>& a);
    bool    equal      (const quat<type>&,const type& eps);


////////////////////////////////////////////////////////////////////////////////////////////
///Основные арифметические операции
////////////////////////////////////////////////////////////////////////////////////////////
    quat&       operator += (const quat& q);
    quat&       operator -= (const quat& q);
    quat&       operator *= (const quat& q);
    quat&       operator *= (const type& a);
    quat&       operator /= (const type& a);
     
    const quat  operator +  (const quat& q) const;
    const quat  operator -  (const quat& q) const;
    const quat  operator *  (const quat& q) const;
    const quat  operator *  (const type& a) const;
    const quat  operator /  (const type& a) const;

    friend const quat operator * (const type& a,const quat& q)  { return q * a; }

////////////////////////////////////////////////////////////////////////////////////////////
///Отношения между кватернионами
////////////////////////////////////////////////////////////////////////////////////////////
    bool operator  ==   (const quat& q) const;
    bool operator  !=   (const quat& q) const;

////////////////////////////////////////////////////////////////////////////////////////////
///Поворот вектора
////////////////////////////////////////////////////////////////////////////////////////////
   const vec<type,4> operator * (const vec<type,4>& v) const;
   const vec<type,3> operator * (const vec<type,3>& v) const;

////////////////////////////////////////////////////////////////////////////////////////////
///Скалярное произведение
////////////////////////////////////////////////////////////////////////////////////////////
   const type operator & (const quat& q) const;
   
////////////////////////////////////////////////////////////////////////////////////////////
///Для корректного использования в данном классе
////////////////////////////////////////////////////////////////////////////////////////////
    using quat_base<type>::x;
    using quat_base<type>::y;
    using quat_base<type>::z;
    using quat_base<type>::w;
};

////////////////////////////////////////////////////////////////////////////////////////////
///Утилиты
////////////////////////////////////////////////////////////////////////////////////////////


///Возвращает единичный кватернион

template <class type>
quat<type> normalize (const quat<type>& q);


///Обратный кватернион

template <class type>
const quat<type> invert (const quat<type>& src);

///Преобразование кватерниона в матрицу

template <class T> const matrix<T,4> quat_to_matrix4 (const quat<T>& q);
template <class T> const matrix<T,3> quat_to_matrix3 (const quat<T>& q);

///Преобразование кватерниона в углы Эйлера

template <class T> void quat_to_axis_angle  (const quat<T>& q,T& angle,vec<T,4>& axis,const T& eps = 1e-03f);
template <class T> void quat_to_axis_angle  (const quat<T>& q,T& angle,vec<T,3>& axis,const T& eps = 1e-03f);
template <class T> void quat_to_euler_angle (const quat<T>& q,T& pitch,T& yaw,T& roll);

////////////////////////////////////////////////////////////////////////////////////////////
///Основные типы
////////////////////////////////////////////////////////////////////////////////////////////

typedef quat<float>              quatf;
typedef quat<double>             quatd;
typedef quat<int>                quati;
typedef quat<unsigned int>       quatui;
typedef quat<short>              quats;
typedef quat<unsigned short>     quatus;
typedef quat<char>               quatb;
typedef quat<unsigned char>      quatub;

#include <math_experimental/impl/quat.inl>

#ifdef _MSC_VER
  #pragma pack (pop)
#endif
}

#endif
