/*
    Конструкторы
*/

template <class T>
inline rect<T>::rect ()
  : x ()
  , y ()
  , width ()
  , height ()
{
}

template <class T>
inline rect<T>::rect (const value_type& in_x, const value_type& in_y, const value_type& in_width, const value_type& in_height)
  : x (in_x)
  , y (in_y)
  , width (in_width)
  , height (in_height)
{
}

template <class T>
inline rect<T>::rect (const rect& r)
  : x (r.x)
  , y (r.y)
  , width (r.width)
  , height (r.height)
{
}

template <class T> template <class T1>
inline rect<T>::rect (const rect<T1>& r)
  : x (static_cast<value_type> (r.x))
  , y (static_cast<value_type> (r.y))
  , width (static_cast<value_type> (r.width))
  , height (static_cast<value_type> (r.height))
{
}

/*
    Присваивание
*/

template <class T>
inline rect<T>& rect<T>::operator = (const rect& r)
{
  x      = r.x;
  y      = r.y;
  width  = r.width;
  height = r.height;

  return *this;
}

template <class T> template <class T1>
inline rect<T>& rect<T>::operator = (const rect<T1>& r)
{
  x      = static_cast<value_type> (r.x);
  y      = static_cast<value_type> (r.y);
  width  = static_cast<value_type> (r.width);
  height = static_cast<value_type> (r.height);

  return *this;
}

/*
    Получение экстент
*/

template <class T>
inline const typename rect<T>::value_type& rect<T>::left () const
{
  return x;
}

template <class T>
inline const typename rect<T>::value_type& rect<T>::top () const
{
  return y;
}

template <class T>
inline typename rect<T>::value_type rect<T>::right () const
{
  return x + width;
}

template <class T>
inline typename rect<T>::value_type rect<T>::bottom () const
{
  return y + height;
}

/*
    Сравнение
*/

template <class T>
inline bool rect<T>::operator == (const rect& r) const
{
  return x == r.x && y == r.y && width == r.width && height == r.height;
}

template <class T>
inline bool rect<T>::operator != (const rect& r) const
{
  return !(*this == r);
}

/*
    Обмен
*/

template <class T>
inline void rect<T>::swap (rect& r)
{
  rect tmp = *this;
  *this    = r;
  r        = tmp;
}

template <class T>
inline void swap (rect<T>& rect1, rect<T>& rect2)
{
  rect1.swap (rect2);
}
