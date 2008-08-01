#ifndef MATHLIB_PLANE_HEADER
#define MATHLIB_PLANE_HEADER

#include "forward.h"
#include "vector.h"
#include "functional.h"
#include <stddef.h>
#include <math.h>
#ifdef _MSC_VER
  #pragma pack (push,1)
#endif

namespace math
{

//Плоскость
template<class Type,size_t Size>
class plane
{
 public:

  typedef Type type;
  enum {size=Size};
  typedef vec<type,size> value_vec;

  enum Side {
   positive=1,
   negative=-1,
   zero=0
  };

  ///////////////////////////////////////////////////////////////////////////////////////////////////
  //Конструкторы
  ///////////////////////////////////////////////////////////////////////////////////////////////////
  plane();//по умолчанию
  plane(const value_vec& normal_vec, const value_vec& control_pnt=value_vec(0));//через точку и нормальный вектор
  plane(const value_vec& point1, const value_vec& point2, const value_vec& point3);//через три точки
  plane(const value_vec& vec1, const value_vec& vec2, const value_vec& point);//через две пересекающиеся прямые
  plane(const value_vec& point1, const value_vec& point2, const value_vec& vec);//через две параллельные прямые
  plane(const plane<type>& pl);//копирование

  ///////////////////////////////////////////////////////////////////////////////////////////////////
  //Возврат значений
  ///////////////////////////////////////////////////////////////////////////////////////////////////
  const value_vec& get_normal_vector () const {return normal_vector; }
  const value_vec& get_control_point () const {return control_point; }



 private:
  value_vec control_point;
  value_vec normal_vector;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
//Утилиты
///////////////////////////////////////////////////////////////////////////////////////////////////

//Расстояние до точки
template<class T,size_t Size>
T& distance(const plane<T,Size>& pl, const vec<T,Size>& point, plane<T,Size>::Side& s);

template<class T,size_t Size>
T& distance(const plane<T,Size>& pl, const vec<T,Size>& point);

/*
	Основные типы
*/
typedef plane<float,3>              plane3f;
typedef plane<double,3>             plane3d;
typedef plane<int,3>                plane3i;
typedef plane<unsigned int,3>       plane3ui;
typedef plane<short,3>              plane3s;
typedef plane<unsigned short,3>     plane3us;
typedef plane<char,3>               plane3b;
typedef plane<unsigned char,3>      plane3ub;

typedef plane<float,4>              plane4f;
typedef plane<double,4>             plane4d;
typedef plane<int,4>                plane4i;
typedef plane<unsigned int,4>       plane4ui;
typedef plane<short,4>              plane4s;
typedef plane<unsigned short,4>     plane4us;
typedef plane<char,4>               plane4b;
typedef plane<unsigned char,4>      plane4ub;

#include "impl/plane.inl"

#ifdef _MSC_VER
  #pragma pack (pop)
#endif
}

#endif
