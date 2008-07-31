#ifndef MATHLIB_PLANE_HEADER
#define MATHLIB_PLANE_HEADER

#include <math_experimental/forward.h>
#include "vector.h"
#include "functional.h"
#include <stddef.h>
#ifdef _MSC_VER
  #pragma pack (push,1)
#endif

namespace math
{

//Точка
template <class Type>
class point
{
 public:
  typedef Type type;
  
  enum {size=3};

  ///////////////////////////////////////////////////////////////////////////////////////////////////
  //Конструкторы
  ///////////////////////////////////////////////////////////////////////////////////////////////////

  point();
  point(const type& x_, const type& y_, const type& z_=T(0));
  point(const point<type>& p);

  ///////////////////////////////////////////////////////////////////////////////////////////////////
  //Индексация (для совместимости с функционалами)
  ///////////////////////////////////////////////////////////////////////////////////////////////////

        type& operator [](size_t index)       {return (&x) [index]; }
  const	type& operator [](size_t index) const {return (&x) [index]; }

  ///////////////////////////////////////////////////////////////////////////////////////////////////
  //Бинарные операторы
  ///////////////////////////////////////////////////////////////////////////////////////////////////

  point<type>& move (const type& x_, const type& y_, const type& z_);
  point<type>& operator= (const point<type>& p);

 private:
  type x, y, z;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
//Утилиты
///////////////////////////////////////////////////////////////////////////////////////////////////

//Создание вектора
template<class T>
const vec<T,3> create_vector(const point<T>& p1,const point<T>& p2);

//Плоскость
template<class Type>
class plane
{
  public:

  typedef Type type;

  ///////////////////////////////////////////////////////////////////////////////////////////////////
  //Конструкторы
  ///////////////////////////////////////////////////////////////////////////////////////////////////
  plane();//по умолчанию
  plane(const vec<type,3>& normal, const point<type>& control=point<type>(0,0,0));//через точку и нормальный вектор
  plane(const point<type>& p1, const point<type>& p2, const point<type>& p3);//через три точки
  plane(const vec<type,3>& v1, const vec<type,3>& v2, const point<type> p);//через две пересекающиеся прямые
  plane(const point<type>& p1, const point<type>& p2,const vec<type,3> v);//через две параллельные прямые
  plane(const plane<type>& p);//копирование

  ///////////////////////////////////////////////////////////////////////////////////////////////////
  //Возврат значений
  ///////////////////////////////////////////////////////////////////////////////////////////////////
  const vec<type,3>& get_vector() const {return normal_vector; }
  const point<type>& get_point () const {return control_point; }

  private:
  point<type> control_point;
  vec<type,3> normal_vector;
};

#include "impl/point.inl"
#include "impl/plane.inl"

#ifdef _MSC_VER
  #pragma pack (pop)
#endif
}

#endif
