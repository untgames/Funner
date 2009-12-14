/*
    Конструкторы
*/

template <class T>
inline axis_aligned_box<T>::axis_aligned_box (const vec_type& vmin, const vec_type& vmax)
  : min_extent (vmin), max_extent (vmax)
  {}

template <class T>
inline axis_aligned_box<T>::axis_aligned_box (const T& min_x, const T& min_y, const T& min_z, const T& max_x, const T& max_y, const T& max_z)
  : min_extent (min_x, min_y, min_z), max_extent (max_x, max_y, max_z)
  {}
  
template <class T>
inline axis_aligned_box<T>::axis_aligned_box (const sphere<T>& s)
  : min_extent (s.center () - vec_type (s.radius ())), max_extent (s.center () + vec_type (s.radius ()))
  {}

/*
    Получение экстент
*/

template <class T>
inline const typename axis_aligned_box<T>::vec_type& axis_aligned_box<T>::minimum () const
{
  return min_extent;
}

template <class T>
inline const typename axis_aligned_box<T>::vec_type& axis_aligned_box<T>::maximum () const
{
  return max_extent;
}

/*
    Установка экстент
*/

template <class T>
inline void axis_aligned_box<T>::set_minimum (const vec_type& vmin)
{
  min_extent = vmin;
}

template <class T>
inline void axis_aligned_box<T>::set_minimum (const element_type& x, const element_type& y, const element_type& z)
{
  set_minimum (vec_type (x, y, z));
}

template <class T>
inline void axis_aligned_box<T>::set_maximum (const vec_type& vmax)
{
  max_extent = vmax;
}

template <class T>
inline void axis_aligned_box<T>::set_maximum (const element_type& x, const element_type& y, const element_type& z)
{
  set_maximum (vec_type (x, y, z));
}

template <class T>
inline void axis_aligned_box<T>::set_extents (const vec_type& vmin, const vec_type& vmax)
{
  set_minimum (vmin);
  set_maximum (vmax);
}

template <class T>
inline void axis_aligned_box<T>::set_extents (const T& min_x, const T& min_y, const T& min_z, const T& max_x, const T& max_y, const T& max_z)
{
  set_extents (vec_type (min_x, min_y, min_z), vec_type (max_x, max_y, max_z));
}

/*
    Сброс объёма
*/

template <class T>
inline void axis_aligned_box<T>::reset (const vec_type& p)
{
  min_extent = max_extent = p;
}

/*
    Получение статистик ограничивающего параллелипиппеда
*/

template <class T>
inline typename axis_aligned_box<T>::vec_type axis_aligned_box<T>::center () const
{
  return (max_extent + min_extent) / T (2);
}

template <class T>
inline typename axis_aligned_box<T>::vec_type axis_aligned_box<T>::size () const
{
  return max_extent - min_extent;
}

template <class T>
inline T axis_aligned_box<T>::radius () const
{
  return length (size ()) / T (2);
}

/*
    Проверка на пустоту
*/

template <class T>
inline bool axis_aligned_box<T>::empty () const
{
  for (size_t i=0; i<3; i++)
    if (min_extent [i] >= max_extent [i])
      return true;

  return false;
}

/*
    Получение координат углов параллелипиппеда
*/

template <class T>
inline typename axis_aligned_box<T>::vec_type axis_aligned_box<T>::corner (box_corner corner) const
{
  switch (corner)
  {
    case box_corner_nxnynz: return min_extent;
    case box_corner_pxnynz: return vec_type (max_extent.x, min_extent.y, min_extent.z);
    case box_corner_nxpynz: return vec_type (min_extent.x, max_extent.y, min_extent.z);
    case box_corner_pxpynz: return vec_type (max_extent.x, max_extent.y, min_extent.z);
    case box_corner_nxnypz: return vec_type (min_extent.x, min_extent.y, max_extent.z);
    case box_corner_pxnypz: return vec_type (max_extent.x, min_extent.y, max_extent.z);
    case box_corner_nxpypz: return vec_type (min_extent.x, max_extent.y, max_extent.z);
    case box_corner_pxpypz: return max_extent;
    default:                return vec_type ();
  }
}

template <class T>
inline void axis_aligned_box<T>::get_corners (vec_type corners [8]) const
{
  corners [box_corner_nxnynz] = min_extent;
  corners [box_corner_pxnynz] = vec_type (max_extent.x, min_extent.y, min_extent.z);
  corners [box_corner_nxpynz] = vec_type (min_extent.x, max_extent.y, min_extent.z);
  corners [box_corner_pxpynz] = vec_type (max_extent.x, max_extent.y, min_extent.z);
  corners [box_corner_nxnypz] = vec_type (min_extent.x, min_extent.y, max_extent.z);
  corners [box_corner_pxnypz] = vec_type (max_extent.x, min_extent.y, max_extent.z);
  corners [box_corner_nxpypz] = vec_type (min_extent.x, max_extent.y, max_extent.z);
  corners [box_corner_pxpypz] = max_extent;
}
    
/*
    Добавление точки в объём / объединение объёмов
*/

template <class T>
inline axis_aligned_box<T>& axis_aligned_box<T>::operator += (const vec_type& v)
{
  for (size_t i=0; i<3; i++)
  {
    if (v [i] < min_extent [i]) min_extent [i] = v [i];
    if (v [i] > max_extent [i]) max_extent [i] = v [i];
  }  
  
  return *this;
}

template <class T>
inline axis_aligned_box<T> axis_aligned_box<T>::operator + (const vec_type& v) const  
{
  return axis_aligned_box (*this) += v;
}

template <class T>
inline axis_aligned_box<T>& axis_aligned_box<T>::operator += (const axis_aligned_box& box)
{
  (*this) += box.min_extent;
  (*this) += box.max_extent;
  
  return *this;
}

template <class T>
inline axis_aligned_box<T> axis_aligned_box<T>::operator + (const axis_aligned_box& box) const  
{
  return axis_aligned_box (*this) += box;
}

template <class T>
inline axis_aligned_box<T>& axis_aligned_box<T>::operator += (const sphere<T>& s)
{
  return (*this) += axis_aligned_box (s);
}

template <class T>
inline axis_aligned_box<T> axis_aligned_box<T>::operator + (const sphere<T>& s) const
{
  return axis_aligned_box (*this) += axis_aligned_box (s);
}

/*
    Преобразования ограничивающего параллелипиппеда
*/

template <class T>
inline axis_aligned_box<T>& axis_aligned_box<T>::operator *= (const math::matrix<T, 4>& tm)
{
  vec_type vmin = min_extent, vmax = max_extent;
  
  min_extent = max_extent = tm * vmin;
  
  (*this) += tm * vmax;
  
  return *this;
}

template <class T>
inline axis_aligned_box<T>& axis_aligned_box<T>::operator *= (const math::quat<T>& q)
{
  vec_type vmin = min_extent, vmax = max_extent;
  
  min_extent = max_extent = q * vmin;
  
  (*this) += q * vmax;
  
  return *this;
}

template <class T>
inline axis_aligned_box<T> operator * (const math::matrix<T, 4>& tm, const axis_aligned_box<T>& box)
{
  return box * tm;
}

template <class T>
inline axis_aligned_box<T> axis_aligned_box<T>::operator * (const math::matrix<T, 4>& tm) const
{
  axis_aligned_box box;
  
  box.min_extent = box.max_extent = tm * min_extent;

  box += tm * max_extent;

  return box;
}

template <class T>
inline axis_aligned_box<T> axis_aligned_box<T>::operator * (const math::quat<T>& q) const
{
  axis_aligned_box box;

  box.min_extent = box.max_extent = q * min_extent;

  box += q * max_extent;

  return box;  
}

template <class T>
inline axis_aligned_box<T> operator * (const math::quat<T>& tm, const axis_aligned_box<T>& box)
{
  return box * tm;
}

/*
    Сравнение
*/

template <class T>
inline bool axis_aligned_box<T>::operator == (const axis_aligned_box& box) const
{
  return min_extent == box.min_extent && max_extent == box.max_extent;
}

template <class T>
inline bool axis_aligned_box<T>::operator != (const axis_aligned_box& box) const
{
  return !(*this == box);
}

/*
    Проверка эквивалентности
*/

template <class T>
inline bool equal (const axis_aligned_box<T>& box1, const axis_aligned_box<T>& box2, const T& eps)
{
  return math::equal (box1.minimum (), box2.minimum (), eps) && math::equal (box1.maximum (), box2.maximum (), eps);
}
    
/*
    Проверка пересечения ограничивающего параллелипиппеда с различными примитивами
*/

template <class T>
inline bool intersects (const axis_aligned_box<T>& box1, const axis_aligned_box<T>& box2)
{
  if (box1.empty () || box2.empty ())
    return false;

  for (size_t i=0; i<3; i++)
    if (box1.maximum ()[i] < box2.minimum ()[i] || box1.minimum ()[i] > box2.maximum ()[i])
      return false;

  return true;  
}

template <class T>
inline bool intersects (const axis_aligned_box<T>& box, const sphere<T>& sphere)
{
  return intersects (sphere, box);
}

/*
    Проверка: содержит ли ограничивающий параллелипиппед различные примитивы
*/

template <class T>
inline bool contains (const axis_aligned_box<T>& box, const math::vector<T, 3>& p)
{
  if (box.empty ())
    return false;

  for (size_t i=0; i<3; i++)
    if (p [i] < box.minimum ()[i] || p [i] > box.maximum ()[i])
      return false;
      
  return true;
}

template <class T>
inline bool contains (const axis_aligned_box<T>& box1, const axis_aligned_box<T>& box2)
{
  if (box1.empty () || box2.empty ())
    return false;

  return contains (box1, box2.minimum ()) && contains (box1, box2.maximum ());
}

template <class T>
inline bool contains (const axis_aligned_box<T>& box, const sphere<T>& s)
{
  if (box.empty () || s.empty ())
    return false;

  if (!contains (box, s.center ()))
    return false;

  return contains (box, normalize (s.center () - box.center () * s.radius () + s.center ()));
}

/*
    Определение области перекрытия двух ограничивающих параллелипиппедов
*/

template <class T>
inline axis_aligned_box<T> intersection (const axis_aligned_box<T>& a, const axis_aligned_box<T>& b)
{
  typename axis_aligned_box<T>::vec_type vmin = a.minimum (), vmax = a.maximum ();
  
  for (size_t i=0; i<3; i++)
  {
    if (b.minimum () [i] > vmin [i]) vmin [i] = b.minimum () [i];
    if (b.maximum () [i] < vmax [i]) vmax [i] = b.maximum () [i];
    
    if (vmin [i] > vmax [i])
      return axis_aligned_box<T> ();
  }
  
  return axis_aligned_box<T> (vmin, vmax);
}

/*
    Получение объёма
*/

template <class T>
inline T volume (const axis_aligned_box<T>& box)
{
  typename axis_aligned_box<T>::vec_type size = box.size ();

  return size.x * size.y * size.z;
}
