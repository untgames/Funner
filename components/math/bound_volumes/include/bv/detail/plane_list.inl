template <class T> struct plane_list<T>::implementation : public xtl::reference_counter
{
  typedef math::plane<T>          plane_type;
  typedef stl::vector<plane_type> plane_array;

  plane_array planes; //Плоскости
};

/*
   Конструкторы / деструктор / копирование
*/

template <class T>
plane_list<T>::plane_list ()
  : impl (new implementation)
  {}

template <class T>
plane_list<T>::plane_list (implementation* new_impl)
  : impl (new_impl)
  {}

template <class T>
plane_list<T>::plane_list (const plane_list& source)
  : impl (source.impl)
{
  xtl::reference_counter* ref_count = impl;  //Обход бага компиляции gcc 4.0 на мак

  addref (ref_count);
}

template <class T>
plane_list<T>::~plane_list ()
{
  release (impl);
}

template <class T>
plane_list<T>& plane_list<T>::operator = (const plane_list& source)
{
  plane_list (source).swap (*this);
  return *this;
}

/*
   Копирование
*/

template <class T>
plane_list<T> plane_list<T>::clone () const
{
  return plane_list (new implementation (*impl));
}

/*
   Получение количества плоскостей/проверка на пустоту
*/

template <class T>
size_t plane_list<T>::planes_count () const
{
  return impl->planes.size ();
}

template <class T>
bool plane_list<T>::empty () const
{
  return impl->planes.empty ();
}

/*
   Резервирование плоскостей
*/

template <class T>
void plane_list<T>::reserve (size_t planes_count)
{
  impl->planes.reserve (planes_count);
}

template <class T>
size_t plane_list<T>::capacity () const
{
  return impl->planes.capacity ();
}

/*
   Получение плоскости
*/

template <class T>
const typename plane_list<T>::plane_type& plane_list<T>::operator [] (size_t index) const
{
  return impl->planes [index];
}

template <class T>
typename plane_list<T>::plane_type& plane_list<T>::operator [] (size_t index)
{
  return impl->planes [index];
}

template <class T>
const typename plane_list<T>::plane_type& plane_list<T>::at (size_t index) const
{
  return const_cast<plane_list<T>&> (*this).at (index);
}

template <class T>
typename plane_list<T>::plane_type& plane_list<T>::at (size_t index)
{
  if (index >= impl->planes.size ())
    throw xtl::make_range_exception ("math::plane_list<T>::at", "index", index, impl->planes.size ());

  return impl->planes [index];
}

/*
   Добавление плоскости
*/

template <class T>
size_t plane_list<T>::add (size_t planes_count, const plane_type* planes)
{
  if (planes_count && !planes)
    throw xtl::make_null_argument_exception ("math::plane_list<T>::add", "planes");

  impl->planes.insert (impl->planes.end (), planes, planes + planes_count);

  return impl->planes.size () - planes_count;
}

template <class T>
size_t plane_list<T>::add (const plane_type& p)
{
  return add (1, &p);
}

/*
   Удаление плоскостей
*/

template <class T>
void plane_list<T>::remove (size_t index, size_t count)
{
  if (index >= impl->planes.size ())
    return;

  count = stl::min (count, impl->planes.size () - index);

  impl->planes.erase (impl->planes.begin () + index, impl->planes.begin () + index + count);
}

template <class T>
void plane_list<T>::clear ()
{
  impl->planes.clear ();
}

/*
   Добавление примитивов в ограничивающий объём
*/

template <class T>
plane_list<T>& plane_list<T>::operator += (const plane_list& p)
{
  if (p.empty ())
    return *this;

  add (p.planes_count (), &p [0]);

  return *this;
}

template <class T>
plane_list<T>& plane_list<T>::operator += (const plane_type& p)
{
  add (p);
  return *this;
}

template <class T>
plane_list<T> plane_list<T>::operator + (const plane_list& p) const
{
  return plane_list (*this) += p;
}

template <class T>
plane_list<T> plane_list<T>::operator + (const plane_type& p) const
{
  return plane_list (*this) += p;
}

/*
   Сравнение
*/

template <class T>
bool plane_list<T>::operator == (const plane_list& p) const
{
  return equal (*this, p, T (0));
}

template <class T>
bool plane_list<T>::operator != (const plane_list& p) const
{
  return !(*this == p);
}

/*
   Обмен
*/

template <class T>
void plane_list<T>::swap (plane_list<T>& p)
{
  stl::swap (impl, p.impl);
}

/*
   Обмен
*/

template <class T>
void swap (plane_list<T>& p1, plane_list<T>& p2)
{
  p1.swap (p2);
}

/*
   Проверка пересечения ограничивающей сферы с различными примитивами
*/

template <class T>
bool contains (const plane_list<T>& p, const math::vector<T, 3>& point, const T& eps)
{
  for (size_t i = 0, count = p.planes_count (); i < count; i++)
    if (distance (p [i], point) > eps)
      return false;

  return true;
}

template <class T>
bool contains (const plane_list<T>& p, const axis_aligned_box<T>& b, const T& eps)
{
  if (b.empty ())
    return false;

  typename axis_aligned_box<T>::vec_type center    = b.center ();
  typename axis_aligned_box<T>::vec_type half_size = b.size () / 2.f;

  for (size_t i = 0, count = p.planes_count (); i < count; i++)
  {
    const math::plane<T>& current_plane = p [i];

    if (-math::distance (current_plane, center) < math::dot (math::abs (current_plane.normal ()), half_size) - eps)
      return false;
  }

  return true;
}

template <class T>
bool contains (const plane_list<T>& p, const sphere<T>& s, const T& eps)
{
  if (s.empty ())
    return false;

  for (size_t i = 0, count = p.planes_count (); i < count; i++)
    if ((math::distance (p [i], s.center ()) + s.radius ()) > eps)
      return false;

  return true;
}

template <class T>
bool intersects (const plane_list<T>& p, const axis_aligned_box<T>& b, const T& eps)
{
  if (b.empty ())
    return false;

  typename axis_aligned_box<T>::vec_type center    = b.center ();
  typename axis_aligned_box<T>::vec_type half_size = b.size () / 2.f;

  for (size_t i = 0, count = p.planes_count (); i < count; i++)
  {
    const math::plane<T>& current_plane = p [i];

    if (math::distance (current_plane, center) > math::dot (math::abs (current_plane.normal ()), half_size) + eps)
      return false;
  }

  return true;
}

template <class T>
bool intersects (const plane_list<T>& p, const sphere<T>& s, const T& eps)
{
  if (s.empty ())
    return false;

  for (size_t i = 0, count = p.planes_count (); i < count; i++)
    if ((math::distance (p [i], s.center ()) - s.radius ()) > eps)
      return false;

  return true;
}

/*
   Проверка эквивалентности
*/

template <class T>
bool equal (const plane_list<T>& p1, const plane_list<T>& p2, const T& eps)
{
  size_t p1_planes_count = p1.planes_count ();

  if (p1_planes_count != p2.planes_count ())
    return false;

  for (size_t i = 0; i < p1_planes_count; i++)
    if (!math::equal (p1 [i], p2 [i], eps))
      return false;

  return true;
}

/*
   Создание по матрице
*/

template <class T>
plane_list<T> make_frustum (const math::matrix<T, 4>& view_projection)
{
  throw xtl::make_not_implemented_exception ("bound_volumes::make_frustum");
}
