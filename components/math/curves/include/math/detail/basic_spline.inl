namespace detail
{

namespace adl
{

///Проверка равенства двух значений
template <class T1, class T2>
bool equal (const T1& a, const T1& b, const T2& eps)
{
  T1 diff = a - b;

  return diff <= eps || -diff <= eps;
}

template <class T1, class T2>
bool check_equal (const T1& a, const T1& b, const T2& eps)
{
  return equal (a, b, eps);
}

}

///Обёртка для инициализации результата сплайна
template <class T> struct spline_result_wrapper
{
  T value;
  
  spline_result_wrapper () : value () {}
};

///Фрейм
template <class Key> struct spline_key_frame
{
  typedef Key                            key_type;
  typedef typename key_type::time_type   time_type;
  typedef typename key_type::scalar_type scalar_type;
  typedef typename key_type::value_type  value_type;
  typedef math::vector<value_type, 4>    factors_type;

  key_type     key;
  time_type    time_factor;
  factors_type factors;
  
  spline_key_frame (const key_type& in_key)
    : key (in_key)
  {
  }
};

template <class T> struct spline_key_frame<spline_step_key<T> >
{
  typedef spline_step_key<T>             key_type;
  typedef typename key_type::time_type   time_type;
  typedef typename key_type::scalar_type scalar_type;
  typedef typename key_type::value_type  value_type;

  key_type key;

  spline_key_frame (const key_type& in_key)
    : key (in_key)
  {
  }
};

template <class T> struct spline_key_frame<spline_linear_key<T> >
{
  typedef spline_linear_key<T>           key_type;
  typedef typename key_type::time_type   time_type;
  typedef typename key_type::scalar_type scalar_type;
  typedef typename key_type::value_type  value_type;

  key_type   key;
  time_type  time_diff;
  value_type value_diff;

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
    return frame.key.time < time;
  }
};

///Нормирование времени
template <class Time, class Frame>
Time normalize_time (const Time& time, const Frame& frame)
{
  return (time - frame.key.time) * frame.time_factor;
}

///Интерполяция
template <class Time, class Frame, class Value>
void spline_eval (const Time& unclamped_time, const Frame& frame, Value& result)
{    
  Time time = normalize_time (unclamped_time, frame), time2 = time * time;

  result = dot (math::vector<Value, 4> (Value (1), Value (time), Value (time2), Value (time2 * time)), frame.factors);
}

template <class Time, class Value>
void spline_eval (const Time&, const spline_key_frame<spline_step_key<Value> >& frame, Value& result)
{
  result = frame.key.value;
}

template <class Time, class Value>
void spline_eval (const Time& time, const spline_key_frame<spline_linear_key<Value> >& frame, Value& result)
{
  result = frame.key.value + frame.value_diff * (time - frame.key.time) / frame.time_diff;
}

///Раcчёт коэффициентов TCB-сплайна
template <class Frame>
void spline_recompute_hermite (const Frame& prev, const Frame& cur, const Frame& next1, const Frame& next2, Frame& result)
{
  typedef typename Frame::value_type   value_type;
  typedef typename Frame::scalar_type  scalar_type;
  typedef typename Frame::time_type    time_type;
  
  static value_type hermite_basis_matrix_factors [16] ={
    value_type ( 1),               0,               0,               0,
                  0,               0, value_type ( 1),               0,
    value_type (-3), value_type ( 3), value_type (-2), value_type (-1),
    value_type ( 2), value_type (-2), value_type ( 1), value_type ( 1),
  };
  
  static math::matrix<value_type, 4> hermite_basis_matrix (hermite_basis_matrix_factors);
  
  const scalar_type &tension    = cur.key.tension,
                    &continuity = cur.key.continuity,
                    &bias       = cur.key.bias;
                    
  static const value_type one (1), two (2);
  
  value_type g  (next1.key.value - cur.key.value),
             t0 (((one-tension)*(one+bias)*(one+continuity) * (cur.key.value - prev.key.value) +
                 (one-tension)*(one-bias)*(one-continuity) * g) / two),
             t1 (((one-tension)*(one+bias)*(one-continuity) * g +
                 (one-tension)*(one-bias)*(one+continuity) * (next2.key.value - next1.key.value)) / two);
  
  result.factors = hermite_basis_matrix * math::vector<value_type, 4> (cur.key.value, next1.key.value, t0, t1);
  
  time_type duration = next1.key.time - cur.key.time;  
  
  static time_type EPS (0.0001f);

  result.time_factor = duration <= EPS ? time_type (0) : time_type (1) / duration;
}

template <class Frame> struct spline_frame_selector
{
  spline_frame_selector (Frame* in_first, Frame* in_last, bool in_closed)
    : first (in_first)
    , last (in_last)
    , count (in_last - in_first)
    , closed (in_closed)
  {
  }
  
  Frame& operator () (Frame* frame) const
  {
    if (frame < first)
    {
      if (!closed)
        return *first;  
        
      return first [(count - (first - frame)) % count];
    }
    
    if (frame >= last)
    {
      if (!closed)
        return last [-1];
        
      return first [(frame - first) % count];
    }
    
    return *frame;
  }

  Frame        *first, *last;
  unsigned int count;
  bool         closed;
};

template <class Frame>
void spline_recompute_hermite (Frame* frame, const spline_frame_selector<Frame>& selector)
{
  spline_recompute_hermite (selector (frame-1), *frame, selector (frame+1), selector (frame+2), *frame);
}

template <class T>
void recompute (spline_key_frame<spline_tcb_key<T> >* first, spline_key_frame<spline_tcb_key<T> >* last, bool closed)
{
  if (first == last)
    return;

  typedef spline_key_frame<spline_tcb_key<T> > frame_type;
  
    //расчёт коэффициентов для средних точек
  
  for (frame_type* frame=first+1; frame<last-2; ++frame)
    spline_recompute_hermite (frame [-1], frame [0], frame [1], frame [2], *frame);
  
    //расчёт коэффициентов для крайних точек
    
  spline_frame_selector<frame_type> frame_selector (first, last, closed);

  spline_recompute_hermite (first, frame_selector);

  if (last - 2 > first)
    spline_recompute_hermite (last - 2, frame_selector);  

  if (last - 1 > first)
    spline_recompute_hermite (last - 1, frame_selector);
}

///Расчёт коэффициентов Безье сплайна
template <class T>
void recompute (spline_key_frame<spline_bezier_key<T> >* first, spline_key_frame<spline_bezier_key<T> >* last, bool closed)
{
  if (first == last)
    return;
    
  typedef spline_key_frame<spline_bezier_key<T> > frame_type;
  typedef typename frame_type::value_type         value_type;
  typedef typename frame_type::time_type          time_type;
    
  static value_type bezier_basis_matrix_factors [16] ={
    value_type ( 1),               0,               0,               0,
    value_type (-3), value_type ( 3),               0,               0,
    value_type ( 3), value_type (-6), value_type ( 3),               0,
    value_type (-1), value_type ( 3), value_type (-3), value_type ( 1),
  };

  static math::matrix<value_type, 4> bezier_basis_matrix (bezier_basis_matrix_factors);  
  
  static time_type EPS (0.0001f);

  for (frame_type* frame=first; frame<last-1; ++frame)
  {
    time_type duration = frame [1].key.time - frame->key.time;
  
    frame->factors     = bezier_basis_matrix * math::vector<value_type, 4> (frame->key.value, frame->key.value + frame->key.outer_value, frame [1].key.value + frame [1].key.inner_value, frame [1].key.value);
    frame->time_factor = duration <= EPS ? time_type (0) : time_type (1) / duration;
  }

  if (last - 1 >= first)
  {
    frame_type* cur = last - 1, *next = closed ? first : cur;
  
    time_type duration = next->key.time - cur->key.time;
  
    cur->factors     = bezier_basis_matrix * math::vector<value_type, 4> (cur->key.value, cur->key.value + cur->key.outer_value, next->key.value + next->key.inner_value, next->key.value);
    cur->time_factor = duration <= EPS ? time_type (0) : time_type (1) / duration;
  } 
}

///Расчёт сплайна без интерполяции
template <class T>
void recompute (spline_key_frame<spline_step_key<T> >* first, spline_key_frame<spline_step_key<T> >* last, bool closed)
{
}

///Расчёт сплайна без интерполяции
template <class T>
void recompute (spline_key_frame<spline_linear_key<T> >* first, spline_key_frame<spline_linear_key<T> >* last, bool closed)
{
  if (first == last)
    return;

  typedef spline_key_frame<spline_linear_key<T> > frame_type;

  for (frame_type* frame=first; frame<last-1; ++frame)
  {
    frame->value_diff = frame [1].key.value - frame->key.value;
    frame->time_diff  = frame [1].key.time - frame->key.time;
  }
    
  if (last - 1 > first)
  {
    last [-1].value_diff = last [-1].key.value - last [-2].key.value;  
    last [-1].time_diff  = last [-1].key.time - last [-2].key.time;
  }
  else
  {
    last [-1].value_diff = 0;  
    last [-1].time_diff  = 1;
  }
}

}

/*
    TCB сплайн
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
  , continuity ()
  , bias ()  
{
}

template <class T>  
spline_tcb_key<T>::spline_tcb_key (const time_type& in_time, const value_type& in_value)
  : base (in_time, in_value)
  , tension ()
  , continuity ()
  , bias ()
{
}

template <class T>
spline_tcb_key<T>::spline_tcb_key (const time_type& in_time, const value_type& in_value, const scalar_type& in_tension, const scalar_type& in_continuity, const scalar_type& in_bias)
  : base (in_time, in_value)
  , tension (in_tension)
  , continuity (in_continuity)
  , bias (in_bias)
{
}

/*
    Сплайн без интерполяции
*/

template <class T>
spline_step_key<T>::spline_step_key ()
{
}

template <class T>
spline_step_key<T>::spline_step_key (const time_type& time, const value_type& value)
  : base (time, value)
{
}

/*
    Сплайн с линейной интерполяцией
*/

template <class T>
spline_linear_key<T>::spline_linear_key ()
{
}

template <class T>
spline_linear_key<T>::spline_linear_key (const time_type& time, const value_type& value)
  : base (time, value)
{
}

/*
    Безье сплайн
*/

template <class T>
spline_bezier_key<T>::spline_bezier_key ()
  : inner_value ()
  , outer_value ()
{
}

template <class T>  
spline_bezier_key<T>::spline_bezier_key (const time_type& in_time, const value_type& in_value)
  : base (in_time, in_value)
  , inner_value ()
  , outer_value ()
{
}

template <class T>  
spline_bezier_key<T>::spline_bezier_key (const time_type& in_time, const value_type& in_value, const value_type& in_inner_value, const value_type& in_outer_value)
  : base (in_time, in_value)
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
  scalar_type closed_eps;       //погрешность для определения закрытости сплайна
  bool        closed;           //закрытый ли сплайн
  bool        need_recompute;   //ключи требуют перерасчёта
  bool        need_sort;        //ключи требуют сортировки

///Конструктор
  implementation ()
    : begin_wrap (spline_wrap_default)
    , end_wrap (spline_wrap_default)
    , min_time ()
    , max_time ()
    , closed_eps (0)
    , closed (false)
    , need_recompute (true)    
    , need_sort (true)
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
      min_time = frames.front ().key.time;
      max_time = frames.back ().key.time;
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
    
    closed = frames.empty () || detail::adl::check_equal (frames.front ().key.value, frames.back ().key.value, closed_eps);
    
    detail::recompute (&*frames.begin (), &*frames.end (), closed);

    need_recompute = false;
  }
  
///Отсечение времени
  time_type clamp (const time_type& time, float coef)
  {
    return static_cast<time_type> (fmod (static_cast<float> (time - min_time), coef * static_cast<float> (max_time - min_time))) + min_time;
  }

///Получение отсеченного согласно настройкам сплайна времени
  time_type get_wrapped_time (time_type time)
  {
      //перерасчёт
    
    if (need_recompute)    
      recompute ();

    if (time < min_time)
    {
      switch (begin_wrap)
      {
        case spline_wrap_clamp:
          return min_time;
        case spline_wrap_repeat:
          return max_time - fmod (static_cast<float> (min_time - time), max_time - min_time);
        case spline_wrap_mirror:
          time = min_time + fmod (static_cast<float> (min_time - time), 2 * (max_time - min_time));
          return time < max_time ? time : 2 * max_time - time;
      }
    }
    
    if (time > max_time)
    {
      switch (end_wrap)
      {
        case spline_wrap_clamp:
          return max_time;
        case spline_wrap_repeat:
          return clamp (time, 1.0f);
        case spline_wrap_mirror:
          time = clamp (time, 2.0f);
          return time < max_time ? time : 2 * max_time - time;
      }
    }

    return time;
  }

///Поиск ключевого кадра
  frame_type& get_frame (time_type time)
  {
      //перерасчёт

    if (need_recompute)
      recompute ();

      //поиск кадра

    typename frame_list::iterator iter = stl::lower_bound (frames.begin (), frames.end (), time, detail::spline_key_frame_less<frame_type> ());
    
    if (iter == frames.end ())
      throw xtl::format_operation_exception ("math::basic_spline<T>::implementation::get_frame", "No frames");
      
    if (time < iter->key.time)
      --iter;

    return *iter;    
  }
};

/*
    Конструкторы / деструктор / присваивание
*/

template <class Key>
basic_spline<Key>::basic_spline ()
  : impl (new implementation)
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
  xtl::reference_counter* ref_count = impl;  //Обход бага компиляции gcc 4.0 на мак

  addref (ref_count);
}

template <class Key>
basic_spline<Key>::~basic_spline ()
{
  release (impl);
}

template <class Key>
basic_spline<Key>& basic_spline<Key>::operator = (const basic_spline& s)
{
  basic_spline (s).swap (*this);
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
  return impl->begin_wrap;
}

template <class Key>
spline_wrap basic_spline<Key>::end_wrap () const
{
  return impl->end_wrap;
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
    Закрытый ли сплайна
*/

template <class Key>
bool basic_spline<Key>::closed () const
{
  impl->recompute ();

  return impl->closed;
}

template <class Key>
void basic_spline<Key>::set_closed_eps (const scalar_type& eps)
{
  impl->closed_eps     = eps;
  impl->need_recompute = true;
}

template <class Key>
const typename basic_spline<Key>::scalar_type& basic_spline<Key>::closed_eps () const
{
  return impl->closed_eps;
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
  impl->need_sort      = true;
}

/*
    Добавление ключа
*/

template <class Key>
size_t basic_spline<Key>::add_keys (size_t keys_count, const key_type* keys)
{
  if (keys_count && !keys)
    throw xtl::make_null_argument_exception ("math::basic_spline<Key>::add_keys", "keys");

  impl->frames.insert (impl->frames.end (), keys, keys + keys_count);

  impl->need_recompute = true;
  impl->need_sort      = true;  
  
  return impl->frames.size () - keys_count;
}

template <class Key>
size_t basic_spline<Key>::add_key (const key_type& key)
{
  return add_keys (1, &key);
}

template <class Key>
size_t basic_spline<Key>::add_key (const time_type& time, const value_type& value)
{
  key_type key (time, value);

  return add_keys (1, &key);
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
  time_type wrapped_time = impl->get_wrapped_time (time);

  typename implementation::frame_type& frame = impl->get_frame (wrapped_time);
  
  detail::spline_eval (wrapped_time, frame, out_value);
}

template <class Key>
typename basic_spline<Key>::value_type basic_spline<Key>::eval (const time_type& time) const
{
  detail::spline_result_wrapper<typename Key::value_type> result;
  
  eval (time, result.value);
  
  return result.value;
}

template <class Key>
typename basic_spline<Key>::value_type basic_spline<Key>::operator () (const time_type& time) const
{
  return eval (time);
}

template <class Key>
void basic_spline<Key>::operator () (const time_type& time, typename basic_spline<Key>::value_type& value) const
{
  eval (time, value);
}

/*
  Минимальное и максимальное время
*/

template <class Key>
const typename basic_spline<Key>::time_type& basic_spline<Key>::min_time () const
{
  impl->sort ();
  
  return impl->min_time;
}

template <class Key>
const typename basic_spline<Key>::time_type& basic_spline<Key>::max_time () const
{
  impl->sort ();
  
  return impl->max_time;
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
