/*
	Конструкторы
*/
template<class T,size_t Size>
inline plane<T,Size>::plane()
{
  control_point=vec<T,Size>(0);
  normal_vector=vec<T,Size>(0);

}

template<class T,size_t Size>
inline plane<T,Size>::plane(const vec<T,Size>& normal_vec, const vec<T,Size>& control_pnt=vec<T,Size>(0))
{
  normal_vector=normal_vec;
  control_point=control_pnt;
}

template<class T,size_t Size>
inline plane<T,Size>::plane(const plane<T,Size>& pl)
{
  normal_vector=pl.get_normal_vector();
  control_point=pl.get_control_point();
}


/*
	Бинарные операторы
*/

template<class T,size_t Size>
bool plane<T,Size>::operator == (const plane<T,Size>& p) const
{
  return (is_parallel(normal_vector,p.get_normal_vector())&&(distance(*this,p.get_control_point())<=T(0.00001)));
}

template<class T,size_t Size>
plane<T,Size>& plane<T,Size>::operator = (const plane<T,Size>& p)
{
  normal_vector=p.get_normal_vector();
  control_point=p.get_control_point();
  return *this;
}

template<class T,size_t Size>
plane<T,Size>& plane<T,Size>::operator *= (const quat<T>& q)
{
  normal_vector=q*normal_vector;
  return *this;
}

template<class T,size_t Size>
const plane<T,Size> plane<T,Size>::operator *(const quat<T>& q) const
{
  return plane(q*normal_vector,control_point);
}


/*
	Утилиты
*/

///Объявление плоскости

template<class T,size_t Size>
inline const plane<T,Size> plane_by_3points(const vec<T,Size>& point1, const vec<T,Size>& point2, const vec<T,Size>& point3)
{
  return plane<T,Size>((point1-point2)^(point2-point3),point1);
}

template<class T,size_t Size>
inline const plane<T,Size> plane_by_2crosslines(const vec<T,Size>& vec1, const vec<T,Size>& vec2, const vec<T,Size>& point)
{
  return plane<T,Size>(vec1^vec2,point);
}

template<class T,size_t Size>
inline const plane<T,Size> plane_by_2parallel_lines(const vec<T,Size>& point1, const vec<T,Size>& point2, const vec<T,Size>& vec)
{
  return plane<T,Size> ((point1-point2)^vec,point1);
}


template<class T,size_t Size>
inline T& distance(const plane<T,Size>& pl, const vec<T,Size>& point, typename plane<T,Size>::Side& s)
{
  T dist((pl.get_normal_vector()&point)/point.length());
  if (dist>T(0))  s=plane<T,Size>::Side::positive;
  if (dist<T(0))  s=plane<T,Size>::Side::negative;
  if (dist==T(0)) s=plane<T,Size>::Side::zero;
  return abs(dist);
}

template<class T,size_t Size>
inline const T distance(const plane<T,Size>& pl, const vec<T,Size>& point)
{
  T dist((pl.get_normal_vector()&point)/point.length());
  return absol<T,T>()(dist);
}
