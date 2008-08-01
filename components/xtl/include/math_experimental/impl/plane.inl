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
inline plane<T,Size>::plane(const vec<T,Size>& normal_vec, const vec<T,Size>& control_pnt)
{
  normal_vector=normal_vec;
  control_point=control_pnt;
}

template<class T,size_t Size>
inline plane<T,Size>::plane(const vec<T,Size>& point1, const vec<T,Size>& point2, const vec<T,Size>& point3)
{
  normal_vector=(point1-point2)^(point2-point3);
  control_point=p1;
}

template<class T,size_t Size>
inline plane<T,Size>::plane(const vec<T,Size>& vec1, const vec<T,Size>& vec2, const vec<T,Size>& point)
{
  normal_vector=vec1^vec2;
  control_point=point;
}

template<class T,size_t Size>
inline plane<T,Size>::plane(const vec<T,Size>& point1, const vec<T,Size>& point2,const vec<T,Size>& vec)
{
  normal_vector=(point1-point2)^vec;
  control_point=point1;
}

template<class T,size_t Size>
inline plane<T,Size>::plane(const plane<T,Size>& p)
{
  normal_vector=p.get_normal_vector();
  control_point=p.get_control_point();
}

/*
	Утилиты
*/
template<class T,size_t Size>
inline T& distance(const plane<T,Size>& pl, const vec<T,Size>& point, plane<T,Size>::Side& s)
{
  T dist((pl.get_normal_vector()&point)/point.length());
  if (dist>T(0))  s=plane<T,Size>::Side::positive;
  if (dist<T(0))  s=plane<T,Size>::Side::negative;
  if (dist==T(0)) s=plane<T,Size>::Side::zero;
  return abs(dist);
}

template<class T,size_t Size>
inline T& distance(const plane<T,Size>& pl, const vec<T,Size>& point)
{
  T dist(pl.get_normal_vector()*point/point.length());
  return abs(dist);
}


