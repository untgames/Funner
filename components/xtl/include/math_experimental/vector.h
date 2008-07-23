#ifndef MATHLIB_VECTOR_HEADER
#define MATHLIB_VECTOR_HEADER

#include <float.h>
#include <math_experimental/forward.h>
#include <stddef.h>
#include <math.h>
#include "functional.h"
#ifdef _MSC_VER
  #pragma pack (push,1)
#endif

#undef min
#undef max




namespace math
{

//forward declaration
////////////////////////////////////////////////////////////////////////////////////////////
///Векторная база по умолчанию
////////////////////////////////////////////////////////////////////////////////////////////
template <class Type, size_t Size> struct vec_base
{
  Type&       operator [] (size_t index)       { return x [index]; }
  const Type& operator [] (size_t index) const { return x [index]; }
 
  Type x [Size];
};

template <class Type> struct vec_base<Type, 2>
{
  Type&       operator [] (size_t index)       { return (&x) [index]; }
  const Type& operator [] (size_t index) const { return (&x) [index]; }
 
  Type x, y;
};

template <class Type> struct vec_base<Type, 3>
{
  Type&       operator [] (size_t index)       { return (&x) [index]; }
  const Type& operator [] (size_t index) const { return (&x) [index]; }
 
  Type x, y, z;
};

template <class Type> struct vec_base<Type, 4>
{
  Type&       operator [] (size_t index)       { return (&x) [index]; }
  const Type& operator [] (size_t index) const { return (&x) [index]; }
 
  Type x, y, z, w;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
//Вектор 
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class Type, size_t Size>
class vec: public vec_base<Type, Size>
{
  public:
    typedef vec_base<Type, Size> base;
    typedef Type                value_type;

    enum { size = Size };

////////////////////////////////////////////////////////////////////////////////////////////
///Конструкторы
////////////////////////////////////////////////////////////////////////////////////////////
    vec ();
    vec (const Type&);
    vec (const base&);

      //это решение - не лучшее,  но оно меня устраивает
    vec (const Type&, const Type&, const Type& = 0, const Type& = 0); 
    vec (const vec<Type, Size-1>&, const Type&);

//////////////////////////////////////////////////////////////////////////////////////////////

  template <size_t Size1,  size_t Size2,  class Fn> vec (const vec<Type, Size1>v1, const vec<Type, Size2>v2, Fn fn);
  template <size_t Size1, class Fn>                 vec (const vec<Type, Size1>& v1, const Type& arg, Fn fn);
  template <size_t Size1, class Fn>                 vec (const vec<Type, Size1>& v1, Fn fn);
  template <class Fn>                               vec (const Type& v1, Fn fn);

  template <class Fn>                               vec (const quat<Type>& q1, const Type& arg, Fn fn);

//////////////////////////////////////////////////////////////////////////////////////////////////
   //временные конструкторы. Удалить после редактирования библиотеки

    template <class T1, class T2> vec (const T1&, const T2&, void (*eval)(vec&, const T1&, const T2&));

    template <size_t Size1>       vec (const vec<Type, Size1>&); 

////////////////////////////////////////////////////////////////////////////////////////////
///Унарный +, -,  длина (length)
////////////////////////////////////////////////////////////////////////////////////////////
    const vec    operator +     () const;
    const vec    operator -     () const;
          Type   length         () const;
          Type   squared_length () const;
////////////////////////////////////////////////////////////////////////////////////////////
///Присваивание
////////////////////////////////////////////////////////////////////////////////////////////
  template<class T> vec&   operator =  (const T&); 
                    vec&   operator =  (const base&);

////////////////////////////////////////////////////////////////////////////////////////////
///Основные бинарные операции
////////////////////////////////////////////////////////////////////////////////////////////
    vec&       operator += (const vec&);
    vec&       operator -= (const vec&);
    const vec  operator +  (const vec&)  const;
    const vec  operator -  (const vec&)  const;


    vec&       operator *= (const vec& );
    vec&       operator *= (const Type&);

    vec&       operator /= (const vec& );
    vec&       operator /= (const Type&);

    const vec  operator *  (const vec& ) const;
    const vec  operator *  (const Type&) const;

    const vec  operator /  (const vec& ) const;
    const vec  operator /  (const Type&) const;


////////////////////////////////////////////////////////////////////////////////////////////
///Отношения между векторами           
////////////////////////////////////////////////////////////////////////////////////////////
    bool operator == (const vec&) const;      
    bool operator != (const vec&) const;
    bool operator <  (const vec&) const;      
    bool operator >  (const vec&) const;
    bool operator <= (const vec&) const;      
    bool operator >= (const vec&) const;

    bool equal       (const vec&, const Type& eps); 


////////////////////////////////////////////////////////////////////////////////////////////
///Cкалярное произведение
////////////////////////////////////////////////////////////////////////////////////////////
    const Type operator &  (const vec&) const;


};

template <class Type> class vec<Type, 0> {};

#ifdef _MSC_VER
  #pragma pack(pop)
#endif

////////////////////////////////////////////////////////////////////////////////////////////
///Бинарные операции
////////////////////////////////////////////////////////////////////////////////////////////

template<class T,size_t Size>
  const vec<T,Size> operator *  (const T& a, const vec<T,Size>& v)    { return v * a; }


////////////////////////////////////////////////////////////////////////////////////////////
///Преобразования
////////////////////////////////////////////////////////////////////////////////////////////

template<class T,size_t Size>
const matrix<T, Size, 1>    to_matrix_column (const vec<T,Size>&);

template<class T,size_t Size>
const matrix<T, 1, Size>    to_matrix_row    (const vec<T,Size>&);

//////////////////////////////////////////////////////////////////////////////////////////////////////
///Нормализация
//////////////////////////////////////////////////////////////////////////////////////////////////////

template<class T,size_t Size>
const vec<T, Size> normalize (const vec<T, Size>&);

//////////////////////////////////////////////////////////////////////////////////////////////////////
///Покомпонентный модуль
//////////////////////////////////////////////////////////////////////////////////////////////////////

template<class T,size_t Size>
const vec<T, Size> abs       (const vec<T, Size>&);

//////////////////////////////////////////////////////////////////////////////////////////////////////
///Векторное произведение
//////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
const vec<T, 3> operator ^ (const vec<T, 3>& a, const vec<T, 3>& b);


//////////////////////////////////////////////////////////////////////////////////////////////////////
///Покомпонентный минимум/максимум
//////////////////////////////////////////////////////////////////////////////////////////////////////
    
template <class T,size_t Size>
const vec<T,Size> min (const vec<T,Size>&,const vec<T,Size>&); 

template <class T,size_t Size>
const vec<T,Size> max (const vec<T,Size>&,const vec<T,Size>&); 

//////////////////////////////////////////////////////////////////////////////////////////////////////
///Угол между векторами
//////////////////////////////////////////////////////////////////////////////////////////////////////
template <class T,size_t Size>
const T angle (const vec<T, Size>& a, const vec<T, Size>& b);

/*
	Основные типы
*/
typedef vec<float,2>              vec2f;
typedef vec<double,2>             vec2d;
typedef vec<int,2>                vec2i;
typedef vec<unsigned int,2>       vec2ui;
typedef vec<short,2>              vec2s;
typedef vec<unsigned short,2>     vec2us;
typedef vec<char,2>               vec2b;
typedef vec<unsigned char,2>      vec2ub;

typedef vec<float,3>              vec3f;
typedef vec<double,3>             vec3d;
typedef vec<int,3>                vec3i;
typedef vec<unsigned int,3>       vec3ui;
typedef vec<short,3>              vec3s;
typedef vec<unsigned short,3>     vec3us;
typedef vec<char,3>               vec3b;
typedef vec<unsigned char,3>      vec3ub;

typedef vec<float,4>              vec4f;
typedef vec<double,4>             vec4d;
typedef vec<int,4>                vec4i;
typedef vec<unsigned int,4>       vec4ui;
typedef vec<short,4>              vec4s;
typedef vec<unsigned short,4>     vec4us;
typedef vec<char,4>               vec4b;
typedef vec<unsigned char,4>      vec4ub;


#include <math_experimental/impl/vector.inl>
}
#endif
