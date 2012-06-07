/*
===================================================================================================
    stat_counter
===================================================================================================
*/

namespace detail
{

namespace stat_counter_adl
{

template <class T>
const char* get_type_name (xtl::type<T>)
{
  return typeid (T).name ();
}

}

template <class T>
const char* get_type_name_impl ()
{
  using namespace stat_counter_adl;
  
  return get_type_name (xtl::type<T> ());
}

}

struct stat_counter::group
{
  stat_counter* first_counter;
  stat_counter* last_counter;
  group*        next_group;
  get_name_fn   get_name;
  
  template <bool dummy> struct container_desc
  {
    static group *first_group, *last_group; 
  };
  
  typedef container_desc<true> container;
  
  group (get_name_fn in_get_name) : get_name (in_get_name)
  {
    if (container::last_group) container::last_group->next_group = this;
    else                       container::first_group            = this;
  
    container::last_group = this;  
  }
};

template <bool dummy> stat_counter::group* stat_counter::group::container_desc<dummy>::first_group = 0; 
template <bool dummy> stat_counter::group* stat_counter::group::container_desc<dummy>::last_group  = 0;

template <class T>
struct stat_counter::group_container: public group
{
  group_container () : group (&detail::get_type_name_impl<T>) {}
};

#ifdef _MSC_VER
  #pragma warning(push)
  #pragma warning(disable : 4624) // 'xtl::stat_counter::counter_container<T,Tag>' : destructor could not be generated because a base class destructor is inaccessible
#endif

template <class T, class Tag>
struct stat_counter::counter_container
{
  stat_counter instance;
  
  counter_container () : instance (xtl::type<T> (), xtl::type<Tag> ()) {}
};

#ifdef _MSC_VER
  #pragma warning(pop)
#endif

/*
    Конструктор
*/

template <class T, class Tag>
inline stat_counter::stat_counter (xtl::type<T>, xtl::type<Tag>)
  : counter_value (0)
  , next_counter (0)
  , owner_group (singleton_default<group_container<Tag> >::instance ())
  , get_name (&detail::get_type_name_impl<T>)
{
  if (owner_group.last_counter) owner_group.last_counter->next_counter = this;
  else                          owner_group.first_counter              = this;

  owner_group.last_counter = this;
}

/*
    Значение счётчика
*/

inline stat_counter::value_type stat_counter::value () const
{
  return counter_value;
}

/*
    Имя счётчика и имя группы
*/

inline const char* stat_counter::name () const
{
  return get_name ();
}

inline const char* stat_counter::group_name () const
{
  return owner_group.get_name ();
}

/*
    Изменение значения счётчика
*/

inline stat_counter& stat_counter::operator += (value_type value)
{
#ifdef XTL_HAS_INTERLOCKED
  atomic_increment (*(volatile int*)&counter_value, (int)value);
#else
  counter_value += value;
#endif

  return *this;
}

inline stat_counter& stat_counter::operator -= (value_type value)
{
#ifdef XTL_HAS_INTERLOCKED
  atomic_decrement (*(volatile int*)&counter_value, (int)value);
#else
  counter_value -= value;
#endif
  
  return *this;
}

/*
    Следующий счётчик
*/

inline stat_counter* stat_counter::next  () const
{
  if (next_counter)
    return next_counter;
    
  if (owner_group.next_group)
    return owner_group.next_group->first_counter;
    
  return 0;
}

inline stat_counter* stat_counter::first ()
{
  if (!group::container::first_group)
    return 0;

  return group::container::first_group->first_counter;
}

/*
    Получение счетчика по типу и тэгу
*/

template <class T, class Tag>
inline stat_counter& stat_counter::instance ()
{
  return singleton_default<counter_container<T, Tag>, false>::instance ().instance;
}

/*
===================================================================================================
    instance_counter
===================================================================================================
*/

template <class T, class Tag>
inline instance_counter<T, Tag>::instance_counter ()
{
  stat_counter::instance<T, Tag> () += 1;
}

template <class T, class Tag>
inline instance_counter<T, Tag>::instance_counter (const instance_counter&)  
{
  stat_counter::instance<T, Tag> () += 1;
}

template <class T, class Tag>
inline instance_counter<T, Tag>::~instance_counter ()
{
  stat_counter::instance<T, Tag> () -= 1;
}

/*
===================================================================================================
    stat_counter_iterator
===================================================================================================
*/

/*
    Конструкторы / присваивание
*/

inline stat_counter_iterator::stat_counter_iterator (bool is_last)
{
  if (is_last) counter = 0;
  else         counter = stat_counter::first ();
}

inline stat_counter_iterator::stat_counter_iterator (const stat_counter_iterator& i)
  : counter (i.counter)
{
}

inline stat_counter_iterator& stat_counter_iterator::operator = (const stat_counter_iterator& i)
{
  counter = i.counter;
  return *this;
}
    
/*
    Проверка на корректность
*/

inline stat_counter_iterator::operator unspecified_bool_type () const
{
  if (counter)
    return &stat_counter_iterator::operator ->;    
    
  return 0;
}

inline bool stat_counter_iterator::operator ! () const
{
  return counter == 0;
}

/*
    Селекторы
*/

inline stat_counter_iterator::reference stat_counter_iterator::operator * () const
{
  return *counter;
}

inline stat_counter_iterator::pointer stat_counter_iterator::operator -> () const
{
  return counter;
}

/*
    Инкремент / декремент
*/

inline stat_counter_iterator& stat_counter_iterator::operator ++ ()
{
  if (counter)
    counter = counter->next ();

  return *this;
}

inline stat_counter_iterator stat_counter_iterator::operator ++ (int)
{
  stat_counter_iterator res = *this;
  
  ++*this;
  
  return res;
}

/*
    Сравнение
*/

inline bool stat_counter_iterator::operator == (const stat_counter_iterator& i) const
{
  return counter == i.counter;
}

inline bool stat_counter_iterator::operator != (const stat_counter_iterator& i) const
{
  return !(*this == i);
}

/*
    Получение итераторов на начало и конец списка групп
*/

inline stat_counter_iterator get_first_stat_counter ()
{
  return stat_counter_iterator ();
}

inline stat_counter_iterator get_last_stat_counter ()
{
  return stat_counter_iterator (true);
}

/*
    Получение счётчиков
*/

template <class T, class Tag>
inline stat_counter& get_stat_counter ()
{
  return stat_counter::instance<T, Tag> ();
}

template <class T>
inline stat_counter& get_perfomance_counter ()
{
  return stat_counter::instance<T, perfomance_counter_tag> ();
}

template <class T>
inline stat_counter& get_instance_counter ()
{
  return stat_counter::instance<T, instance_counter_tag> ();
}
