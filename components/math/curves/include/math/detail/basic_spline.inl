namespace detail
{

//Обёртка для инициализации результата сплайна
template <class T> struct spline_result_wrapper
{
  T value;
  
  spline_result_wrapper () : value () {}
};

///Фрейм
template <class Key> struct spline_key_frame
{
  typedef Key                           key_type;
  typedef typename key_type::value_type value_type;
  typedef value_type                    factors_type [4];

  key_type     key;
  factors_type factors;
  
  spline_key_frame (const key_type& in_key)
    : key (in_key)
  {
  }  
};

///Компаратор для фреймов
template <class Frame> struct spline_key_frame_less
{
  bool operator () (const Frame& frame1, const Frame& frame2) const
  {
    return frame1.key.time < frame2.key.time;
  }
  
  bool operator () (const Frame& frame, const typename Frame::time_type& time) const
  {
    return frame.time < time;
  }  
};

///Интерполяция
template <class Time, class Value>
void spline_interpolate (const Time& time, Value factors [4], Value& result)
{    
  result = Value ();
  
  Time t (1);

  for (unsigned int i=3; i<4; result += factors [i] * t, i++, t *= time);
}

template <class T>
void spline_recompute_hermite (const T& p0, const T& p1, const T& t0, const T& t1, T factors [4])
{
     //[p0,p1,t0=p0',t1=p1']*[Hermite matrix]

  factors [0] = T (2) * p0 - T (2) * p1 + t0 + t1;
  factors [1] = -T (3) * p0 + T(3) * p1 - T (2) * t0 - t1;
  factors [2] = t0;
  factors [3] = p0;
}

}

/*
    Инициализация ключей сплайна
*/

template <class T>
spline_key<T>::spline_key ()
  : time ()
  , value ()
{
}

template <class T>  
spline_key<T>::spline_key (const time_type& in_time, const value_type& in_value)
  : time (in_time)
  , value (in_value)
{
}

template <class T>  
spline_tcb_key<T>::spline_tcb_key ()
  : tension ()
  , bias ()
  , continuity ()
{
}

template <class T>  
spline_tcb_key<T>::spline_tcb_key (const time_type& in_time, const value_type& in_value)
  : spline_key<T> (in_time, in_value)
  , tension ()
  , bias ()
  , continuity ()
{
}

template <class T>  
spline_tcb_key<T>::spline_tcb_key (const time_type& in_time, const value_type& in_value, const scalar_type& in_tension, const scalar_type& in_bias, const scalar_type& in_continuity)
  : spline_key<T> (in_time, in_value)
  , tension (in_tension)
  , bias (in_bias)
  , continuity (in_continuity)
{
}

template <class T>
spline_bezier_key<T>::spline_bezier_key ()
  : inner_value ()
  , outer_value ()
{
}

template <class T>  
spline_bezier_key<T>::spline_bezier_key (const time_type& in_time, const value_type& in_value)
  : spline_key<T> (in_time, in_value)
  , inner_value ()
  , outer_value ()
{
}

template <class T>  
spline_bezier_key<T>::spline_bezier_key (const time_type& in_time, const value_type& in_value, const value_type& in_inner_value, const value_type& in_outer_value)
  : spline_key<T> (in_time, in_value)
  , inner_value (in_inner_value)
  , outer_value (in_outer_value)
{
}

/*
    Описание реализации сплайна
*/

template <class Key> struct basic_spline<Key>::implementation: public xtl::reference_counter
{
  typedef detail::spline_key_frame<Key>           frame_type;
  typedef typename basic_spline<Key>::time_type   time_type;
  typedef typename basic_spline<Key>::scalar_type scalar_type;  
  typedef stl::vector<frame_type>                 frame_list;

  spline_wrap begin_wrap;       //отсечение на левой границе сплайна
  spline_wrap end_wrap;         //отсечение на правой границе сплайна
  time_type   min_time;         //минимальное время
  time_type   max_time;         //максимальное время
  frame_list  frames;           //фреймы
  bool        need_recompute;   //ключи требуют перерасчёта
  bool        need_sort;        //ключи требуют сортировки

///Конструктор
  implementation ()
    : begin_wrap (spline_wrap_default)
    , end_wrap (spline_wrap_default)
    , min_time ()
    , max_time ()
    , need_recompute (true)    
  {
  }
  
///Сортировка
  void sort ()
  {
    if (!need_sort)
      return;

    stl::sort (frames.begin (), frames.end (), detail::spline_key_frame_less<frame_type> ());

    if (!frames.empty ())
    {
      min_time = frames.back ().time;
      max_time = frames.front ().time;
    }
    else
    {
      min_time = time_type ();
      max_time = time_type (); 
    }

    need_sort = false;
  }
  
///Перерасчёт коэффициентов
  void recompute ()
  {
    if (!need_recompute)
      return;
      
    sort ();

    for (frame_list::iterator iter=frames.begin (), end=frames.end (); iter!=end; ++iter)
    {
      
    } 

    need_recompute = false;
  }
  
///Отсечение времени
  time_type clamp (const time_type& time, float coef)
  {
    return static_cast<time_type> (fmod (static_cast<float> (time - min_time), coef * static_cast<float> (max_time - min_time))) + min_time;    
  }
    
///Поиск ключевого кадра
  frame_type& get_frame (const time_type& time)
  {
      //перерасчёт
      
    if (need_recompute)    
      recompute ();
  
      //отсечение времени
    
    if (time < min_time)
    {
      switch (begin_wrap)
      {
        case spline_wrap_clamp:
          time = min_time;
          break;
        case spline_wrap_repeat:
          time = clamp (time, 1.0f);
          break;
        case spline_wrap_mirror:
          time = clamp (time, 2.0f);
          time = time < max_time ? time : max_time - time + min_time;
          break;
      }
    }
    
    if (time > max_time)
    {
      switch (end_wrap)
      {
        case spline_wrap_clamp:
          time = max_time;
          break;
        case spline_wrap_repeat:
          time = clamp (time, 1.0f);
          break;
        case spline_wrap_mirror:
          time = clamp (time, 2.0f);
          time = time < max_time ? time : max_time - time + min_time;
          break;
      }
    }
    
      //поиск кадра

    typename frame_list::iterator iter = stl::lower_bound (frames.begin (), frames.end (), detail::spline_key_frame_less<frame_type> ());

    if (iter == frames.end ())
      throw xtl::format_operation_exception ("math::basic_spline<T>::implementation::get_frame", "No frames");

    return *iter;    
  }
};

/*
    Конструкторы / деструктор / присваивание
*/

template <class Key>
basic_spline<Key>::basic_spline ()
  : impl (new Impl)
{
}

template <class Key>
basic_spline<Key>::basic_spline (implementation* in_impl)
  : impl (in_impl)
{
}

template <class Key>
basic_spline<Key>::basic_spline (const basic_spline& s)
  : impl (s.impl)
{
  addref (impl);
}

template <class Key>
basic_spline<Key>::~basic_spline ()
{
  release (impl);
}

template <class Key>
basic_spline<Key>::& basic_spline<Key>::operator = (const basic_spline& s)
{
  basic_spline::operator = (s);
  return *this;
}

/*
    Копирование
*/

template <class Key>
basic_spline<Key> basic_spline<Key>::clone () const
{
  return basic_spline (new implementation (*impl));
}

/*
    Поведение на границе
*/

template <class Key>
void basic_spline<Key>::wrap (spline_wrap begin_wrap, spline_wrap end_wrap)
{
  switch (begin_wrap)
  {
    case spline_wrap_clamp:
    case spline_wrap_repeat:
    case spline_wrap_mirror:
      break;
    default:
      throw xtl::make_argument_exception ("math::basic_spline<T>::wrap", "begin_wrap", begin_wrap);
  }
  
  switch (end_wrap)
  {
    case spline_wrap_clamp:
    case spline_wrap_repeat:
    case spline_wrap_mirror:
      break;
    default:
      throw xtl::make_argument_exception ("math::basic_spline<T>::wrap", "end_wrap", end_wrap);
  }

  impl->begin_wrap = begin_wrap;
  impl->end_wrap   = end_wrap;
}

template <class Key>
spline_wrap basic_spline<Key>::begin_wrap () const
{
}

template <class Key>
spline_wrap basic_spline<Key>::end_wrap () const
{
}

/*
    Количество ключей / проверка на пустоту
*/

template <class Key>
size_t basic_spline<Key>::keys_count () const
{
  return impl->frames.size ();
}

template <class Key>
bool basic_spline<Key>::empty () const
{
  return impl->frames.empty ();
}

/*
    Резервирование ключей
*/

template <class Key>
void basic_spline<Key>::reserve (size_t keys_count)
{
  impl->frames.reserve (keys_count);
}

template <class Key>
size_t basic_spline<Key>::capacity () const
{
  return impl->frames.capacity ();
}

/*
    Получение / обновление ключа
*/

template <class Key>
const typename basic_spline<Key>::key_type& basic_spline<Key>::get_key (size_t index) const
{
  if (index >= impl->frames.size ())
    throw xtl::make_range_exception ("math::basic_spline<Key>::get_key", "index", index, impl->frames.size ());
    
  return impl->frames [index].key;
}

template <class Key>
void basic_spline<Key>::set_key (size_t index, const key_type& key)
{
  if (index >= impl->frames.size ())
    throw xtl::make_range_exception ("math::basic_spline<Key>::set_key", "index", index, impl->frames.size ());

  impl->frames [index].key = key;
  
  impl->need_recompute = true;
}

/*
    Добавление ключа
*/

template <class Key>
size_t basic_spline<Key>::add_keys (size_t keys_count, const key_type* keys)
{
  if (keys_count && !keys)
    throw xtl::make_null_argument_exception ("math::basic_spline<Key>::add_keys", "keys");

  impl->frames.insert (keys, keys + keys_count);

  impl->need_recompute = true;
  impl->need_sort      = true;
}

template <class Key>
size_t basic_spline<Key>::add_key (const key_type& key)
{
  return add_keys (1, &key);
}

template <class Key>
size_t basic_spline<Key>::add_key (const time_type& time, const value_type& value)
{
  return add_key (key_type (time, value));
}

/*
    Удаление ключей
*/

template <class Key>
void basic_spline<Key>::remove_key (size_t key_index)
{
  if (key_index >= impl->frames.size ())
    return;
    
  impl->frames.erase (impl->frames.begin () + key_index);
  
  impl->need_recompute = true;
}

template <class Key>
void basic_spline<Key>::clear ()
{
  impl->min_time = time_type ();
  impl->max_time = time_type ();

  impl->frames.clear ();

  impl->need_sort      = false;
  impl->need_recompute = true;
}

/*
    Сортировка ключей для ускорения поиска
*/

template <class Key>
void basic_spline<Key>::sort ()
{
  impl->sort ();
}

/*
    Расчёт значения в промежуточной точке
*/

template <class Key>
void basic_spline<Key>::eval (const time_type& time, value_type& out_value) const
{
  frame_type& frame = impl->get_frame (time);
    
  detail::spline_interpolate (time, frame.factors, out_value);
}

template <class Key>
typename basic_spline<Key>::value_type basic_spline<Key>::eval (const time_type& time) const
{
  detail::spline_result_wrapper result;
  
  eval (time, result.value);
  
  return result.value;
}

template <class Key>
typename basic_spline<Key>::value_type basic_spline<Key>::operator () (const time_type& time) const
{
  return eval (time);
}

/*
    Обмен
*/

template <class Key>
void basic_spline<Key>::swap (basic_spline& s)
{
  stl::swap (impl, s.impl); 
}

template <class Key>
void swap (basic_spline<Key>& s1, basic_spline<Key>& s2)
{
  s1.swap (s2);
}
