/*
    Конструкторы
*/

template <class T>
inline sphere<T>::sphere ()
  : sphere_radius (-1)
  {}

template <class T>
inline sphere<T>::sphere (const vec_type& center, const element_type& radius)
  : sphere_center (center), sphere_radius (radius)
  {}

/*
    Получение параметров
*/

template <class T>
inline const typename sphere<T>::vec_type& sphere<T>::center () const
{
  return sphere_center;
}

template <class T>
inline typename sphere<T>::element_type sphere<T>::radius () const
{
  return sphere_radius;
}

/*
    Установка параметров
*/

template <class T>
inline void sphere<T>::set_radius (const element_type& r)
{
  sphere_radius = r;
}

template <class T>
inline void sphere<T>::set_center (const vec_type& c)
{
  sphere_center = c;
}

/*
    Сброс объёма
*/

template <class T>
inline void sphere<T>::reset (const vec_type& center, const element_type& radius)
{
  sphere_center = center;
  sphere_radius = radius;
}

/*
    Получение статистик ограничивающей сферы
*/

//геометрический объём сферы
template <class T>
inline typename sphere<T>::element_type sphere<T>::volume () const
{
  static const element_type pi = (element_type)3.1415926;

  return element_type (4) * pi * sphere_radius * sphere_radius * sphere_radius / element_type (3);
}

/*
    Проверка на пустоту
*/

template <class T>
inline bool sphere<T>::empty (const element_type& eps) const
{
  return sphere_radius < 0;
}

/*
    Добавление примитивов в ограничивающий объём
*/

//если текущий радиус < 0 - сбрасываем положение сферы
template <class T>
inline sphere<T>& sphere<T>::operator += (const vec_type& point)
{
  if (sphere_radius < element_type (0))
  {
    sphere_radius = element_type (0);
    sphere_center = point;

    return *this;
  }
  
  element_type distance = length (sphere_center - point);

  if (distance > sphere_radius)
    sphere_radius = distance;

  return *this;
}

template <class T>
inline sphere<T>& sphere<T>::operator += (const sphere<T>& s)
{
  if (sphere_radius < element_type (0))
    return *this = s;
    
  element_type distance = length (s.sphere_center - sphere_center) + s.sphere_radius;

  if (distance > sphere_radius)
    sphere_radius = distance;
    
  return *this;
}

template <class T>
inline sphere<T>& sphere<T>::operator += (const axis_aligned_box<T>& box)
{
  (*this) += box.minimum ();
  (*this) += box.maximum ();
  
  return *this;
}

template <class T>
inline sphere<T> sphere<T>::operator + (const vec_type& vec) const
{
  return sphere (*this) += vec;
}

template <class T>
inline sphere<T> sphere<T>::operator + (const sphere<T>& s) const
{
  return sphere (*this) += s;
}

template <class T>
inline sphere<T> sphere<T>::operator + (const axis_aligned_box<T>& box) const
{
  return sphere (*this) += box;
}

/*
    Преобразования ограничивающей сферы
*/

template <class T>
inline sphere<T>& sphere<T>::operator *= (const math::quat<T>& q)
{
  sphere_center *= q;

  return *this;
}

template <class T>
inline sphere<T> sphere<T>::operator * (const math::quat<T>& q) const
{
  return sphere (*this) *= q;
}

template <class T>
inline sphere<T> operator * (const math::quat<T>& q, const sphere<T>& s)
{
  return s * q;
}

template <class T>
inline axis_aligned_box<T> sphere<T>::operator * (const math::matrix<T, 4>& m) const
{
  return axis_aligned_box (sphere_center - vec_type (sphere_center), sphere_center + vec_type (sphere_center)) *= m;
}

template <class T>
inline axis_aligned_box<T> operator * (const math::matrix<T, 4>& m, const sphere<T>& s)
{
  return s * m;
}

/*
    Проверка пересечения ограничивающей сферы с различными примитивами
*/

template <class T>
inline bool sphere<T>::intersects (const sphere<T>& s) const
{
  float distance = sphere_radius + s.sphere_radius;

  return qlen (sphere_center - sphere_center) < distance * distance;
}

//bool template <class T> class sphere::intersects (const axis_aligned_box<T>&) const;

/*
    Проверка: содержит ли ограничивающая сфера различные примитивы
*/

template <class T>
inline bool typename sphere<T>::contains (const vec_type& point) const
{
  return qlen (point - sphere_center) < sphere_radius * sphere_radius;
}

template <class T>
inline bool typename sphere<T>::contains (const sphere& s) const
{
  return s.sphere_radius + length (sphere_center - s.sphere_center) < sphere_radius;
}

//bool template <class T> class sphere::contains (const axis_aligned_box<T>& box) const;

/*
    Сравнение
*/

//специализировать для float/double/int
template <class T>
inline bool sphere<T>::equal (const sphere& s, const element_type& eps) const
{
  element_type difference = sphere_radius - s.sphere_radius;

  if (difference < element_type (0))
    difference = -difference;

  return math::equal (sphere_center, s.sphere_center, eps) && difference < eps;
}

template <class T>
inline bool sphere<T>::operator == (const sphere<T>& s) const
{
  return sphere_center == s.sphere_center && sphere_radius == s.sphere_radius;
}

template <class T>
inline bool typename sphere<T>::operator != (const sphere& s) const
{
  return !(*this == s);
}
