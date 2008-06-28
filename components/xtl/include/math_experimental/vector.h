#ifndef __MATHLIB_VECTOR__
#define __MATHLIB_VECTOR__

#include <stddef.h>

#ifdef _MSC_VER
  #pragma pack (push,1)
#endif

#undef min
#undef max




namespace math
{


template <class Type,size_t SizeX,size_t SizeY> class matrix;

////////////////////////////////////////////////////////////////////////////////////////////
///Векторная база по умолчанию
////////////////////////////////////////////////////////////////////////////////////////////
template <class Type,size_t Size> struct vec_base
{
  Type&       operator [] (size_t index)       { return x [index]; }
  const Type& operator [] (size_t index) const { return x [index]; }
 
  Type x [Size];
};

template <class Type> struct vec_base<Type,2>
{
  Type&       operator [] (size_t index)       { return (&x) [index]; }
  const Type& operator [] (size_t index) const { return (&x) [index]; }
 
  Type x,y;
};

template <class Type> struct vec_base<Type,3>
{
  Type&       operator [] (size_t index)       { return (&x) [index]; }
  const Type& operator [] (size_t index) const { return (&x) [index]; }
 
  Type x,y,z;
};

template <class Type> struct vec_base<Type,4>
{
  Type&       operator [] (size_t index)       { return (&x) [index]; }
  const Type& operator [] (size_t index) const { return (&x) [index]; }
 
  Type x,y,z,w;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
//Вектор 
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class Type,size_t Size>
class vec: public vec_base<Type,Size>
{
  public:
    typedef vec_base<Type,Size> base;
    typedef Type                value_type;

    enum { size = Size };

////////////////////////////////////////////////////////////////////////////////////////////
///Конструкторы
////////////////////////////////////////////////////////////////////////////////////////////
    vec ();
    vec (const Type&);
    vec (const base&);

      //это решение - не лучшее, но оно меня устраивает
    vec (const Type&,const Type&,const Type& = 0,const Type& = 0); 
    vec (const vec<Type,Size-1>&,const Type&);

//////////////////////////////////////////////////////////////////////////////////////////////

  template <size_t Size1, size_t Size2, class Fn>
				 vec (const vec<Type,Size1>v1,const vec<Type,Size2>v2,Fn fn);
  template <size_t Size1,class Fn>
				 vec (const vec<Type,Size1>& v1,const Type& arg,Fn fn);
  template <size_t Size1,class Fn>
				 vec (const vec<Type,Size1>& v1,Fn fn);
  template <class Fn>
				 vec (const Type& v1,Fn fn);

//////////////////////////////////////////////////////////////////////////////////////////////////


    template <class T1,class T2> vec (const T1&,const T2&,void (*eval)(vec&,const T1&,const T2&));

    template <size_t Size1>         vec (const vec<Type,Size1>&); 

////////////////////////////////////////////////////////////////////////////////////////////
///Унарный +,-, длина (~)
////////////////////////////////////////////////////////////////////////////////////////////
    const vec   operator +  () const;
    const vec    operator -  () const;
          Type   length  () const;
          Type   squared_length() const;
////////////////////////////////////////////////////////////////////////////////////////////
///Присваивание
////////////////////////////////////////////////////////////////////////////////////////////
  template<class T> vec&   operator =  (const T&); 
    vec&   operator =  (const base&);

////////////////////////////////////////////////////////////////////////////////////////////
///Основные бинарные операции
////////////////////////////////////////////////////////////////////////////////////////////
    vec&      operator += (const vec&);
    vec&      operator -= (const vec&);
    const vec operator +  (const vec&) const;
    const vec operator -  (const vec&) const;


    template <class T2>
       vec&      operator *= (const T2&);

    template <class T2>
       vec&      operator /= (const T2&);

    template <class T2>
       const vec operator *  (const T2&) const;

    template <class T2>
       const vec operator /  (const T2&) const;


    friend const vec operator *  (const Type& a,const vec& v)    { return v * a; }

////////////////////////////////////////////////////////////////////////////////////////////
///Умножение вектора строки на матрицу столбец
////////////////////////////////////////////////////////////////////////////////////////////
    vec&      operator *= (const matrix<Type,Size,Size>&);
    vec&      operator *= (const matrix<Type,Size+1,Size+1>&);

    const vec operator *  (const matrix<Type,Size,Size>&) const;
    const vec operator *  (const matrix<Type,Size+1,Size+1>&) const;

////////////////////////////////////////////////////////////////////////////////////////////
///Отношения между векторами           
////////////////////////////////////////////////////////////////////////////////////////////
    bool operator == (const vec&) const;      
    bool operator != (const vec&) const;
    bool operator <  (const vec&) const;      
    bool operator >  (const vec&) const;
    bool operator <= (const vec&) const;      
    bool operator >= (const vec&) const;

    bool equal (const vec&,const Type& eps); 


////////////////////////////////////////////////////////////////////////////////////////////
///Cкалярное произведение
////////////////////////////////////////////////////////////////////////////////////////////
    Type operator &  (const vec&) const;

////////////////////////////////////////////////////////////////////////////////////////////
///Приведение типов (понижение размерности вектора)
////////////////////////////////////////////////////////////////////////////////////////////
    operator const vec<Type,Size-1>& () const;
    operator       vec<Type,Size-1>& ();
    
    const matrix<Type,Size,1> to_matrix_column() const;

    const matrix<Type,1,Size> to_matrix_row() const;

/*
	Утилиты
*/

    const vec<Type,Size> normalize () const;

    const vec<Type,Size> abs (); 
/*
	Дружественные функции
*/

////////////////////////////////////////////////////////////////////////////////////////////
///Векторное произведение
////////////////////////////////////////////////////////////////////////////////////////////

    template <class T>
      friend  vec<T,3> operator ^ (const vec<T,3>& a,const vec<T,3>& b);


////////////////////////////////////////////////////////////////////////////////////////////
///Покомпонентный минимум/максимум
////////////////////////////////////////////////////////////////////////////////////////////                

      friend const vec<Type,Size> min (const vec<Type,Size>&,const vec<Type,Size>&); 


      friend const vec<Type,Size> max (const vec<Type,Size>&,const vec<Type,Size>&); 

////////////////////////////////////////////////////////////////////////////////////////////
///Угол между векторами
////////////////////////////////////////////////////////////////////////////////////////////

      friend Type angle (const vec<Type,Size>& a,const vec<Type,Size>& b);


};

template <class Type> class vec<Type,0> {};

#ifdef _MSC_VER
  #pragma pack(pop)
#endif

////////////////////////////////////////////////////////////////////////////////////////////
///Векторное произведение
////////////////////////////////////////////////////////////////////////////////////////////

template <class Type>
vec<Type,4> operator ^ (const vec<Type,4>& a,const vec<Type,4>& b);

}

////////////////////////////////////////////////////////////////////////////////////////////
///Вспомогательные операции
////////////////////////////////////////////////////////////////////////////////////////////
namespace detail
{
template <class T, size_t Size> T& get_component (math::vec<T, Size>& v, size_t index);

template <class T, size_t Size> const T& get_component (const math::vec<T, Size>& v, size_t index);
}


#endif
