/*
    Конструкторы
*/

template <class T>
inline axis_aligned_box<T>::axis_aligned_box (const vec_type& min, const vec_type& max)
  : min_extent (min), max_extent (max)
  {}

template <class T>
inline axis_aligned_box<T>::axis_aligned_box (const T& min_x, const T& min_y, const T& min_z, const T& max_x, const T& max_y, const T& max_z)
  : min_extent (min_x, min_y, min_z), max_extent (max_x, max_y, max_z)
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
inline void axis_aligned_box<T>::set_minimum (const vec_type& min)
{
  min_extent = min;
}

template <class T>
inline void axis_aligned_box<T>::set_minimum (const element_type& x, const element_type& y, const element_type& z)
{
  set_minimum (vec_type (x, y, z));
}

template <class T>
inline void axis_aligned_box<T>::set_maximum (const vec_type& max)
{
  max_extent = max;
}

template <class T>
inline void axis_aligned_box<T>::set_maximum (const element_type& x, const element_type& y, const element_type& z)
{
  set_maximum (vec_type (x, y, z));
}

template <class T>
inline void axis_aligned_box<T>::set_extents (const vec_type& min, const vec_type& max)
{
  set_minimum (min);
  set_maximum (max);
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

template <class T>
inline T axis_aligned_box<T>::volume () const
{
  vec_type size = this->size ();

  return size.x * size.y * size.z;
}

/*
    Проверка на пустоту
*/

template <class T>
inline bool axis_aligned_box<T>::empty (const element_type& eps) const
{
  vec_type size = abs (this->size ());

  return size.x < eps || size.y < eps || size.z < eps;
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

/*
    Преобразования ограничивающего параллелипиппеда
*/

template <class T>
inline axis_aligned_box<T>& axis_aligned_box<T>::operator *= (const math::matrix<T, 4>& tm)
{
  vec_type min = min_extent, max = max_extent;
  
  min_extent = max_extent = tm * min;
  
  (*this) += tm * max;
  
  return *this;
}

template <class T>
inline axis_aligned_box<T>& axis_aligned_box<T>::operator *= (const math::quat<T>& q)
{
  vec_type min = min_extent, max = max_extent;
  
  min_extent = max_extent = q * min;
  
  (*this) += q * max;
  
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
  
  box.min_extent = box.max_extent = tm * min_extent.

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
    Проверка пересечения ограничивающего параллелипиппеда с различными примитивами
*/

template <class T>
inline bool axis_aligned_box<T>::intersects (const axis_aligned_box& box) const
{
  for (size_t i=0; i<3; i++)
    if (max_extent [i] < box.min_extent [i] || min_extent [i] > box.max_extent [i])
      return false;

  return true;  
}

/*
    Проверка: содержит ли ограничивающий параллелипиппед различные примитивы
*/

template <class T>
inline bool axis_aligned_box<T>::contains (const vec_type& p) const
{
  for (size_t i=0; i<3; i++)
    if (p [i] < min_extent [i] || p [i] > max_extent [i])
      return false;
      
  return true;
}

template <class T>
inline bool axis_aligned_box<T>::contains (const axis_aligned_box& box) const
{
  return contains (box.min_extent) && contains (box.max_extent);
}

/*
    Сравнение
*/

template <class T>
inline bool axis_aligned_box<T>::equal (const axis_aligned_box& box, const element_type& eps) const
{
  return math::equal (min_extent, box.min_extent, eps) && math::equal (max_extent, box.max_extent, eps);
}

template <class T>
inline bool axis_aligned_box<T>::operator == (const axis_aligned_box& box) const
{
  return equal (box);
}

template <class T>
inline bool axis_aligned_box<T>::operator != (const axis_aligned_box& box) const
{
  return !(*this == box);
}

/*
    Определение области перекрытия двух ограничивающих параллелипиппедов
*/

template <class T>
inline axis_aligned_box<T> intersection (const axis_aligned_box<T>& a, const axis_aligned_box<T>& b)
{
  axis_aligned_box<T>::vec_type min = a.minimum (), max = a.maximum ();
  
  for (size_t i=0; i<3; i++)
  {
    if (b.minimum () [i] > min [i]) min [i] = b.minimum () [i];
    if (b.maximum () [i] < max [i]) max [i] = b.maximum () [i];
    
    if (min [i] > max [i])
      return axis_aligned_box<T> ();
  }
  
  return axis_aligned_box<T> (min, max);
}
