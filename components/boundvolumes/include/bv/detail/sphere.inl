/*
   Конструкторы
*/

template <class T>
inline sphere<T>::sphere()
  : sphere_center (vec_type(0,0,0)), sphere_radius (0)
  {}

template <class T>
inline sphere<T>::sphere (const vec_type& center, const element_type& radius)
  : sphere_center (center), sphere_radius (radius)
  {}

/*
   Получение параметров
*/

template <class T>
inline const typename sphere<T>::vec_type &sphere<T>::center () const
{
  return sphere_center;
}

/*template <class T>
const typename sphere<T>::vec_type& sphere<T>::center () const
{
   return center;
} */

template <class T>
inline typename sphere<T>::element_type sphere<T>::radius () const
{
  return sphere_radius;
}

/*
   Установка параметров
*/

template <class T>
inline void typename sphere<T>::set_radius (const element_type& r)
{
  sphere_radius = r;
}

template <class T>
inline void typename sphere<T>::set_center (const vec_type& c)
{
  sphere_center = c;
}

/*
   Сброс объёма
*/

template <class T>
inline void typename sphere<T>::reset (const vec_type& center = vec_type (0), const element_type& radius = element_type (0))
{
  sphere_center = center;
  sphere_radius = radius;
}

/*
   Получение статистик ограничивающей сферы
*/

template <class T>
inline typename sphere<T>::element_type typename sphere<T>::volume () const //геометрический объём сферы
{
  return (element_type)(4.0 * M_PI * sphere_radius * sphere_radius * sphere_radius / 3.0); //както не красиво
}

/*
   Проверка на пустоту
*/

template <class T>
inline bool sphere<T>::empty (const element_type& eps) const
//bool typename sphere<T>::empty (const typename sphere::element_type& eps = default_epsilon) //проверка: r < eps
{
  return sphere_radius < eps;//true:false;
}

/*
   Добавление примитивов в ограничивающий объём
   Если текущий радиус < 0 - сбрасываем положение сферы
*/

template <class T>
inline sphere<T>& sphere<T>::operator += (const vec_type& point)
{
  vec_type vec;
  if (sphere_radius == 0)
  {
    sphere_center = vec;
    return *this;
  }
  if (contains (point))
    return *this;
  vec = (sphere_center + point) / 2;
  sphere_center = vec + normalize (vec) * (sphere_radius / 2);
  return *this;
}

template <class T>
inline sphere<T>& sphere<T>::operator += (const sphere<T>& sph)
{
  vec_type vec;
  if (sphere_radius == 0)
  {
    sphere_radius = sph.radius ();
    sphere_center = sph.centre ();
    return *this;
  }
  if (contains (sph))
    return *this;
  vec = (sphere_center + sph.center ()) / 2;
  sphere_center = vec + normalize (vec) * ((sphere_radius + sph.radius) / 2);
  return *this;
}

//sphere& template <class T> class sphere::operator += (const axis_aligned_box<T>&); //???
template <class T>
inline sphere<T> sphere<T>::operator +  (const vec_type& vec) const
{
  return shere (*this) += vec;
}

template <class T>
inline sphere<T> sphere<T>::operator +  (const sphere<T>& sph) const
{
  return shere (*this) += sph;
}
//sphere  template <class T> class sphere::operator +  (const axis_aligned_box&) const; //???

/*
   Проверка пересечения ограничивающей сферы с различными примитивами
*/

template <class T>
inline bool sphere<T>::intersects (const sphere<T>& sphere) const
{
  return length (sphere_center - sphere.center ()) < sphere_radius + sphere.radius ();
}
//bool template <class T> class sphere::intersects (const axis_aligned_box<T>&) const;

/*
   Проверка: содержит ли ограничивающая сфера различные примитивы
*/

template <class T>
inline bool typename sphere<T>::contains (const vec_type& point) const
{
   return length (point - sphere_center) < sphere_radius;
}

template <class T>
inline bool typename sphere<T>::contains (const sphere& sphere) const
{
   return length (sphere_center - sphere.center ()) < (sphere_radius - 2 * sphere.radius ());   //вроде умножение не нужно
}
//bool template <class T> class sphere::contains (const axis_aligned_box<T>& box) const;

/*
   Сравнение
*/

template <class T>
inline bool sphere<T>::equal (const sphere& sp, const element_type& eps = default_epsilon) const
{
   return ((abs (sphere_radius - sp.radius ()) < eps) && (sphere_center == sp.center ()));
}

template <class T>
inline bool typename sphere<T>::operator == (const sphere<T>& sp) const
{
   return equal (sp);
}

template <class T>
inline bool typename sphere<T>::operator != (const sphere& sp) const
{
   return !equal (sp);
}
