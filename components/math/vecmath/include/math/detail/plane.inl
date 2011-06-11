/*
   Конструкторы
*/

template <class T>
plane<T>::plane ()
  : a (0), b (0), c (1), d (0)
  {}

template <class T>
plane<T>::plane (const vec_type& normal, const value_type& in_d)
  : a (normal.x), b (normal.y), c (normal.z), d (in_d)
  {}

template <class T>
plane<T>::plane (const value_type& in_a, const value_type& in_b, const value_type& in_c, const value_type& in_d)
  : a (in_a), b (in_b), c (in_c), d (in_d)
  {}

/*
   Получение параметров
*/

template <class T>
const typename plane<T>::vec_type& plane<T>::normal () const
{
  return const_cast<plane<T>&> (*this).normal ();
}

template <class T>
typename plane<T>::vec_type& plane<T>::normal ()
{
  return *reinterpret_cast<vec_type*> (&a);
}

/*
   Индексирование
*/

template <class T>
typename plane<T>::value_type& plane<T>::operator [] (unsigned int index)
{
  return (&a) [index];
}

template <class T>
const typename plane<T>::value_type& plane<T>::operator [] (unsigned int index) const
{
  return (&a) [index];
}

/*
   Сравнение
*/

template <class T>
bool plane<T>::operator == (const plane& p) const
{
  for (unsigned int i = 0; i < 4; i++)
    if ((*this)[i] != p [i])
      return false;

  return true;
}

template <class T>
bool plane<T>::operator != (const plane& p) const
{
  return !(*this == p);
}

/*
   Проверка эквивалентности
*/

template <class T>
bool equal (const plane<T>& p1, const plane<T>& p2, const T& eps)
{
  for (unsigned int i = 0; i < 4; i++)
  {
    T diff = p1 [i] - p2 [i];

    if (diff > eps || diff < -eps)
      return false;
  }

  return true;
}

/*
   Возвращает нормированную плоскость
*/

template <class T>
plane<T> normalize (const plane<T>& p)
{
  T inv_normal_length = T (1) / length (p.normal ());

  return plane<T> (p.normal () * inv_normal_length, p.d * inv_normal_length);
}

/*
   Возвращает дистанцию от точки до плоскости
*/

template <class T>
typename plane<T>::value_type distance (const plane<T>& p, const vector<T, 3>& point)
{
  return dot (p.normal (), point) + p.d;
}

/*
   Возвращает проекцию вектора на плоскость
*/

template <class T>
vector<T, 3> project (const plane<T>& p, const vector<T, 3>& v)
{
  const vector<T, 3>& normal = p.normal ();

  matrix<T, 3> projection_matrix;

  projection_matrix [0][0] = T (1) - normal.x * normal.x;
  projection_matrix [0][1] = -normal.x * normal.y;
  projection_matrix [0][2] = -normal.x * normal.z;
  projection_matrix [1][0] = -normal.y * normal.x;
  projection_matrix [1][1] = T (1) - normal.y * normal.y;
  projection_matrix [1][2] = -normal.y * normal.z;
  projection_matrix [2][0] = -normal.z * normal.x;
  projection_matrix [2][1] = -normal.z * normal.y;
  projection_matrix [2][2] = T (1) - normal.z * normal.z;

  return projection_matrix * v;
}

/*
   Возвращает с какой стороны плоскости находится точка
*/

template <class T>
plane_side side (const plane<T>& p, const vector<T, 3>& point, const T& eps)
{
  T d = distance (p, point);

  if (d < -eps)
    return plane_side_negative;

  if (d > eps)
    return plane_side_positive;

  return plane_side_none;
}
