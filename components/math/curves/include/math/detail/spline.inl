namespace detail
{

//Интерфейс операций для различных типов параметров сплайна
class spline_operations
{
  public:  
    typedef basic_spline::scalar_type scalar_type;
  
    virtual void initialize_keys   (size_t keys_count, const void* source_data, void* uninitialized_keys) = 0;
    virtual void destroy_keys      (size_t keys_count, void* keys) = 0;
    virtual void accumulate_values (size_t members_count, const scalar_type* coefs, const void* values, void* uninitialized_result) = 0;

  protected:
    virtual ~spline_operations () {}
};

//Обёртка для инициализации результата сплайна
template <class T> struct spline_result_wrapper
{
  T value;
  
  spline_result_wrapper () : value () {}
};

//Реализация операций для различных типов параметров сплайна
template <class Key> struct spline_operations_impl
{
  public:
    void initialize_keys (spline_type type, size_t keys_count, const void* source_data, void* uninitialized_key)
    {
      const Key* src = static_cast<const Key*> (source);
      Key*       dst = static_cast<Key*> (destination);
      
      try
      {
        for (; keys_count--; src++, dst++)
          new (dst) spline_tcb_key (*src);
      }
      catch (...)
      {
        destroy (dst - static_cast<Key*> (destination), destination);
        throw;
      }
    }

    void destroy_keys (size_t keys_count, void* keys)
    {
      Key* key = static_cast<Key*> (keys);

      for (;keys_count--; ++key)
        key->~Key ();
    }
    
    void accumulate_values (size_t members_count, const scalar_type* coefs, const void* values, void* uninitialized_result)
    {
      spline_result_wrapper<typename Key::value_type> result;

      for (size_t i=0; i<members_count; i++)
        result.value += values [i] * coefs [i];
        
      new (uninitialized_result) (result);
    }
};

}

/*
    Конструкторы / деструктор / присваивание
*/

template <class T, spline_type SplineType>
spline<T, SplineType>::spline ()
  : basic_spline (singleton_default<detail::spline_operations_impl<key_type>, false>::instance (), SplineType, sizeof (key_type))
{
}

template <class T, spline_type SplineType>
spline<T, SplineType>::spline (const spline& s)
  : basic_spline (s)
{
}

template <class T, spline_type SplineType>
spline<T, SplineType>::~spline ()
{
}

template <class T, spline_type SplineType>
spline<T, SplineType>::& spline<T, SplineType>::operator = (const spline& s)
{
  basic_spline::operator = (s);
  return *this;
}

/*
    Копирование
*/

template <class T, spline_type SplineType>
spline<T, SplineType> basic_spline<T, SplineType>::clone () const
{
  return static_cast<spline<T, SplineType> (basic_spline::clone ());
}

/*
    Получение / обновление ключа
*/

template <class T, spline_type SplineType>
const typename spline<T, SplineType>::key_type& spline<T, SplineType>::get_key (size_t index) const
{
  return static_cast<const key_type*> (get_keys ())[index];
}

template <class T, spline_type SplineType>
void spline<T, SplineType>::set_key (size_t index, const key_type& key)
{
  static_cast<key_type*> (get_keys ()) [index] = key;
  
  update_key (index);
}

/*
    Добавление ключа
*/

template <class T, spline_type SplineType>
size_t spline<T, SplineType>::add_keys (size_t keys_count, const key_type* keys)
{
  return basic_spline::add_keys (keys_count, keys);
}

template <class T, spline_type SplineType>
size_t spline<T, SplineType>::add_key (const key_type& key)
{
  return add_keys (1, &key);
}

template <class T, spline_type SplineType>
size_t spline<T, SplineType>::add_key (const time_type& time, const value_type& value)
{
  return add_key (key_type (time, value));
}

/*
    Расчёт значения в промежуточной точке
*/

template <class T, spline_type SplineType>
void spline<T, SplineType>::eval (const time_type& time, value_type& out_value) const
{
  basic_spline::eval (time, &value_type);
}

template <class T, spline_type SplineType>
typename spline<T, SplineType>::value_type spline<T, SplineType>::eval (const time_type& time) const
{
  detail::spline_result_wrapper result;
  
  eval (time, result.value);
  
  return result.value;
}

template <class T, spline_type SplineType>
typename spline<T, SplineType>::value_type spline<T, SplineType>::operator () (const time_type& time) const
{
  return eval (time);
}

/*
    Обмен
*/

template <class T, spline_type SplineType>
void spline<T, SplineType>::swap (spline& s)
{
  basic_spline::swap (s);
}

template <class T, spline_type SplineType>
void swap (spline<T, SplineType>& s1, spline<T, SplineType>& s2)
{
  s1.swap (s2);
}
