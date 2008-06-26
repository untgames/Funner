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


template <class type,size_t sizeX,size_t sizeY> class matrix;

////////////////////////////////////////////////////////////////////////////////////////////
///Векторная база по умолчанию
////////////////////////////////////////////////////////////////////////////////////////////
template <class type,size_t size> struct vec_base
{
  type&       operator [] (size_t index)       { return x [index]; }
  const type& operator [] (size_t index) const { return x [index]; }
 
  type x [size];
};

template <class type> struct vec_base<type,2>
{
  type&       operator [] (size_t index)       { return (&x) [index]; }
  const type& operator [] (size_t index) const { return (&x) [index]; }
 
  type x,y;
};

template <class type> struct vec_base<type,3>
{
  type&       operator [] (size_t index)       { return (&x) [index]; }
  const type& operator [] (size_t index) const { return (&x) [index]; }
 
  type x,y,z;
};

template <class type> struct vec_base<type,4>
{
  type&       operator [] (size_t index)       { return (&x) [index]; }
  const type& operator [] (size_t index) const { return (&x) [index]; }
 
  type x,y,z,w;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
//Вектор 
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class type,size_t size>
class vec: public vec_base<type,size>
{
  public:
    typedef vec_base<type,size> base;
    typedef type                value_type;

    enum { Size = size };

////////////////////////////////////////////////////////////////////////////////////////////
///Конструкторы
////////////////////////////////////////////////////////////////////////////////////////////
    vec ();
    vec (const type&);
    vec (const base&);

      //это решение - не лучшее, но оно меня устраивает
    vec (const type&,const type&,const type& = 0,const type& = 0); 
    vec (const vec<type,size-1>&,const type&);

      //для использования оптимизации возвращаемого значения
//  template <class T>           vec (const T&,void (*eval)(vec&,const T&));


//////////////////////////////////////////////////////////////////////////////////////////////

  template <size_t size1, size_t size2, class Fn>
				 vec (const vec<type,size1>v1,const vec<type,size2>v2,Fn fn);
  template <size_t size1,class Fn>
				 vec (const vec<type,size1>& v1,const type& arg,Fn fn);
  template <size_t size1,class Fn>
				 vec (const vec<type,size1>& v1,Fn fn);
  template <class Fn>
				 vec (const type& v1,Fn fn);

//////////////////////////////////////////////////////////////////////////////////////////////////


    template <class T1,class T2> vec (const T1&,const T2&,void (*eval)(vec&,const T1&,const T2&));

    template <size_t size1>         vec (const vec<type,size1>&); 

////////////////////////////////////////////////////////////////////////////////////////////
///Унарный +,-, длина (~)
////////////////////////////////////////////////////////////////////////////////////////////
    const vec   operator +  () const;
    const vec    operator -  () const;
          type   length  () const;
          type   squared_length() const;
////////////////////////////////////////////////////////////////////////////////////////////
///Присваивание
////////////////////////////////////////////////////////////////////////////////////////////
  template<class T> vec&   operator =  (const T&); 
    vec&   operator =  (const base&);

//    template <size_t size1> vec& operator = (const vec<type,size1>&); 

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


    friend const vec operator *  (const type& a,const vec& v)    { return v * a; }

////////////////////////////////////////////////////////////////////////////////////////////
///Умножение вектора строки на матрицу столбец
////////////////////////////////////////////////////////////////////////////////////////////
    vec&      operator *= (const matrix<type,size,size>&);
    vec&      operator *= (const matrix<type,size+1,size+1>&);

    const vec operator *  (const matrix<type,size,size>&) const;
    const vec operator *  (const matrix<type,size+1,size+1>&) const;

////////////////////////////////////////////////////////////////////////////////////////////
///Отношения между векторами           
////////////////////////////////////////////////////////////////////////////////////////////
    bool operator == (const vec&) const;      
    bool operator != (const vec&) const;
    bool operator <  (const vec&) const;      
    bool operator >  (const vec&) const;
    bool operator <= (const vec&) const;      
    bool operator >= (const vec&) const;

    bool equal (const vec&,const type& eps); 


////////////////////////////////////////////////////////////////////////////////////////////
///Cкалярное произведение
////////////////////////////////////////////////////////////////////////////////////////////
    type operator &  (const vec&) const;

////////////////////////////////////////////////////////////////////////////////////////////
///Приведение типов (понижение размерности вектора)
////////////////////////////////////////////////////////////////////////////////////////////
    operator const vec<type,size-1>& () const;
    operator       vec<type,size-1>& ();
    
    const matrix<type,size,1> to_matrix_column() const;

    const matrix<type,1,size> to_matrix_row() const;

/*
	Утилиты
*/

    const vec<type,size> normalize () const;

    const vec<type,size> abs (); 
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

      friend const vec<type,size> min (const vec<type,size>&,const vec<type,size>&); 


      friend const vec<type,size> max (const vec<type,size>&,const vec<type,size>&); 

////////////////////////////////////////////////////////////////////////////////////////////
///Угол между векторами
////////////////////////////////////////////////////////////////////////////////////////////

      friend type angle (const vec<type,size>& a,const vec<type,size>& b);


};

template <class type> class vec<type,0> {};

#ifdef _MSC_VER
  #pragma pack(pop)
#endif

////////////////////////////////////////////////////////////////////////////////////////////
///Векторное произведение
////////////////////////////////////////////////////////////////////////////////////////////

template <class type>
vec<type,4> operator ^ (const vec<type,4>& a,const vec<type,4>& b);

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
