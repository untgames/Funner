

///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструкторы
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T>
inline frustum<T>::frustum ( )
                           :frustum_origin(0),
                            frustum_direction(0),
                            frustum_near_plane(-1),
                            frustum_far_plane(-1),
                            frustum_width(-1),
                            frustum_height(-1)
{ }

template <class T>
inline frustum<T>::frustum(const frustum<T>::vec_type& _origin,
                           const frustum<T>::vec_type& _direction,
                           frustum<T>::value_type      _near_plane,
                           frustum<T>::value_type      _far_plane,
                           frustum<T>::value_type      _width,
                           frustum<T>::value_type      _height)
                           
                          :frustum_origin(_origin),
                           frustum_direction(_direction),
                           frustum_near_plane(_near_plane),
                           frustum_far_plane(_far_plane),
                           frustum_width(_width),
                           frustum_height(_height)
{ }
         
template <class T>
inline frustum<T>::frustum(const frustum<T>& _frustum)
                          :frustum_origin(_frustum.frustum_origin),
                           frustum_direction(_frustum.frustum_direction),
                           frustum_near_plane(_frustum.frustum_near_plane),
                           frustum_far_plane(_frustum.frustum_far_plane),
                           frustum_width(_frustum.frustum_width),
                           frustum_height(_frustum.frustum_height)
{ }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Сброс объёма
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T>
inline void frustum<T>::reset (const frustum<T>::vec_type&  _origin,
                               const frustum<T>::vec_type&  _direction,
                               frustum<T>::value_type       _width,
                               frustum<T>::value_type       _height,
                               frustum<T>::value_type       _near_plane,
                               frustum<T>::value_type       _far_plane)
{
  frustum_origin      = _origin;
  frustum_direction   = _direction;
  frustum_near_plane  = _near_plane;
  frustum_far_plane   = _far_plane;
  frustum_width       = _width;
  frustum_height      = _height;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение параметров
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T>
inline const frustum<T>::vec_type& frustum<T>::origin()      const
{
  return frustum_origin;
}

template <class T>
inline const frustum<T>::vec_type& frustum<T>::direction()   const
{
  return frustum_direction;
}

template <class T>
inline frustum<T>::value_type      frustum<T>::width()       const
{
  return frustum_width;
}

template <class T>
inline frustum<T>::value_type      frustum<T>::height()      const
{
  return frustum_height;
}

template <class T>
inline frustum<T>::value_type      frustum<T>::near_plane()  const
{
  return frustum_near_plane;
}

template <class T>
inline frustum<T>::value_type      frustum<T>::far_plane()   const
{
  return frustum_far_plane;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///Установка параметров
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T>
inline void frustum<T>::set_origin     (const frustum<T>::vec_type& _origin)
{
  frustum_origin = _origin;
}

template <class T>
inline void frustum<T>::set_origin     (frustum<T>::value_type _origin_x,
                                        frustum<T>::value_type _origin_y,
                                        frustum<T>::value_type _origin_z)
{
  set_origin( vec_type(_origin_x, _origin_y, _origin_z) );
}

template <class T>
inline void frustum<T>::set_direction  (const frustum<T>::vec_type& _direction)
{
  frustum_direction = _direction;
}

template <class T>
inline void frustum<T>::set_direction  (frustum<T>::value_type _direction_x,
                                        frustum<T>::value_type _direction_y,
                                        frustum<T>::value_type _direction_z)
{
  set_direction( vec_type(_direction_x, _direction_y, _direction_z) );
}

template <class T>
inline void frustum<T>::set_width      (frustum<T>::value_type _width)
{
  frustum_width = _width;
}

template <class T>
inline void frustum<T>::set_height     (frustum<T>::value_type _height)
{
  frustum_height = _height;
}

template <class T>
inline void frustum<T>::set_size       (frustum<T>::value_type _width,
                                        frustum<T>::value_type _height)
{
  set_width   (_width);
  set_height  (_height);
}

template <class T>
inline void frustum<T>::set_near_plane (frustum<T>::value_type _near_plane)
{
  frustum_near_plane = _near_plane;
}

template <class T>
inline void frustum<T>::set_far_plane  (frustum<T>::value_type _far_plane)
{
  frustum_far_plane = _far_plane;
}

template <class T>
inline void frustum<T>::set_planes     (frustum<T>::value_type _near_plane,
                                        frustum<T>::value_type _far_plane)
{
  set_near_plane  (_near_plane);
  set_far_plane   (_far_plane);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///Проверка на пустоту
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T>
inline bool frustum<T>::empty() const
{
  return  frustum_direction   ==  vec_type(0) ||
          frustum_width       <   value_type(0) ||
          frustum_height      <   value_type(0) ||
          frustum_near_plane  >   frustum_far_plane ||
          frustum_near_plane  <   value_type(0) ||
          frustum_far_plane   <   value_type(0);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение объёма
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T>
frustum<T>::value_type frustum<T>::volume () const
{
  if (frustum_width       <=  value_type(0) ||
      frustum_height      <=  value_type(0) ||
      frustum_near_plane  <   value_type(0) ||
      frustum_far_plane   <=  value_type(0) ||
      frustum_far_plane   <=  frustum_near_plane)
    return value_type(0);
  
  value_type far_plane  = ;
  
  return value_type(frustum_width * frustum_height / 3.0f *
                    (frustum_far_plane - (frustum_near_plane / frustum_far_plane) *
                                         (frustum_near_plane / frustum_far_plane) *
                                          frustum_near_plane)
                   );
  
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///Сравнение
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T>
bool    frustum<T>::operator ==  (const frustum<T>& _frustum) const
{
  return  frustum_origin      == _frustum.frustum_origin &&
          frustum_direction   == _frustum.frustum_direction &&
          frustum_width       == _frustum.frustum_width &&
          frustum_height      == _frustum.frustum_height &&
          frustum_near_plane  == _frustum.frustum_near_plane &&
          frustum_far_plane   == _frustum.frustum_far_plane;
}

template <class T>
bool    frustum<T>::operator !=  (const frustum<T>& _frustum) const
{
  return !(*this == _frustum);
}

