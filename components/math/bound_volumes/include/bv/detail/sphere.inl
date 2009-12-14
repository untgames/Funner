namespace detail
{

//возведение в квадрат
template <class T>
inline T sqr (const T& x)
{
  return x * x;
}

}

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

template <class T>
inline void sphere<T>::set_center (const element_type& x, const element_type& y, const element_type& z)
{
  set_center (vec_type (x, y, z));
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
    Проверка на пустоту
*/

template <class T>
inline bool sphere<T>::empty () const
{
  return sphere_radius < element_type (0);
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
  sphere_center = q * sphere_center;

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
  return axis_aligned_box<T> (*this) *= m;
}

template <class T>
inline axis_aligned_box<T> operator * (const math::matrix<T, 4>& m, const sphere<T>& s)
{
  return s * m;
}

/*
    Сравнение
*/

template <class T>
inline bool sphere<T>::operator == (const sphere<T>& s) const
{
  return sphere_center == s.sphere_center && sphere_radius == s.sphere_radius;
}

template <class T>
inline bool sphere<T>::operator != (const sphere& s) const
{
  return !(*this == s);
}

/*
    Проверка пересечения ограничивающей сферы с различными примитивами
*/

template <class T>
inline bool intersects (const sphere<T>& s1, const sphere<T>& s2)
{
  if (s1.empty () || s2.empty ())
    return false;

  return qlen (s1.center () - s2.center ()) < detail::sqr (s1.radius () + s2.radius ());
}

//a Simple Method for Box-Sphere Intersection Testing by Jim Arvo from "Graphics Gems", Academic Press, 1990
template <class T>
inline bool intersects (const sphere<T>& s, const axis_aligned_box<T>& box)
{
  if (s.empty () || box.empty ())
    return false;

  T min_distance = T (0);

  for (size_t i=0; i<3; i++)
  {
    T d1 = s.center ()[i] - box.minimum ()[i], d2 = s.center ()[i] - box.maximum ()[i];
                 
    if      (d1 < T (0)) min_distance += detail::sqr (d1);
    else if (d2 > T (0)) min_distance += detail::sqr (d2);  
  }

  return min_distance < detail::sqr (s.radius ());
}

/*
    Проверка: содержит ли ограничивающая сфера различные примитивы
*/

template <class T>
inline bool contains (const sphere<T>& s, const math::vector<T, 3>& point)
{
  return qlen (point - s.center ()) < detail::sqr (s.radius ());
}

template <class T>
inline bool contains (const sphere<T>& s1, const sphere<T>& s2)
{
  if (s1.empty () || s2.empty ())
    return false;

  return s2.radius () + length (s1.center () - s2.center ()) < s1.radius ();
}

template <class T>
inline bool contains (const sphere<T>& s, const axis_aligned_box<T>& box)
{
  if (s.empty () || box.empty ())
    return false;

  return contains (s, box.minimum ()) && contains (s, box.maximum ());
}

/*
    Расчёт объёма сферы
*/

template <class T>
inline T volume (const sphere<T>& s)
{
  static const T pi = T (3.1415926);

  return T (4) * pi * detail::sqr (s.radius ()) * s.radius () / T (3);
}

/*
    Проверка на эквивалентность
*/

template <class T>
inline bool equal (const sphere<T>& s1, const sphere<T>& s2, const T& eps)
{
  T difference = s1.radius () - s2.radius ();

  if (difference < T (0))
    difference = -difference;

  return math::equal (s1.center (), s2.center (), eps) && difference < eps;
}
