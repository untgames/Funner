/*
===================================================================================================
    instance_counter_group
===================================================================================================
*/

namespace detail
{

template <bool dummy> struct instance_counter_group_container
{
  static instance_counter_group *first, *last;
};

template <bool dummy> instance_counter_group *instance_counter_group_container<dummy>::first = 0, *instance_counter_group_container<dummy>::last = 0;

template <class T> class typed_instance_counter_group: public instance_counter_group
{
  public:
    typed_instance_counter_group () : instance_counter_group (typeid (T)) {}
};

}

/*
    Конструктор
*/

inline instance_counter_group::instance_counter_group (const std::type_info& in_type)
  : std_type (in_type)
  , next (0)
{
  if (detail::instance_counter_group_container<true>::last) detail::instance_counter_group_container<true>::last->next = this;
  else                                                      detail::instance_counter_group_container<true>::first      = this;
  
  detail::instance_counter_group_container<true>::last = this;
}

/*
    Количество экземпляров в группе
*/

inline size_t instance_counter_group::size () const
{
  return use_count () - 1;
}

/*
    Тип, соответствующий группе
*/

inline const std::type_info& instance_counter_group::type () const
{
  return std_type;
}

/*
    Получение группы по типу
*/

template <class T>
inline instance_counter_group& get_instance_counter_group ()
{
  return singleton_default<detail::typed_instance_counter_group<T>, false>::instance ();
}

/*
===================================================================================================
    instance_counter
===================================================================================================
*/

template <class T>
inline instance_counter<T>::instance_counter ()
{
  get_instance_counter_group<T> ().increment ();
}

template <class T>
inline instance_counter<T>::instance_counter (const instance_counter&)  
{
  get_instance_counter_group<T> ().increment ();
}

template <class T>
inline instance_counter<T>::~instance_counter ()
{
  get_instance_counter_group<T> ().decrement ();
}

template <class T>
inline instance_counter<T>& instance_counter<T>::operator = (const instance_counter&)
{
  return *this;
}

/*
===================================================================================================
    instance_counter_group_iterator
===================================================================================================
*/

/*
    Конструкторы / присваивание
*/

inline instance_counter_group_iterator::instance_counter_group_iterator (bool is_last)
{
  if (is_last) group = 0;
  else         group = detail::instance_counter_group_container<true>::first;  
}

inline instance_counter_group_iterator::instance_counter_group_iterator (const instance_counter_group_iterator& i)
  : group (i.group)
{
}

inline instance_counter_group_iterator& instance_counter_group_iterator::operator = (const instance_counter_group_iterator& i)
{
  group = i.group;
  return *this;
}
    
/*
    Проверка на корректность
*/

inline instance_counter_group_iterator::operator unspecified_bool_type () const
{
  if (group)
    return &instance_counter_group_iterator::operator ->;    
    
  return 0;
}

inline bool instance_counter_group_iterator::operator ! () const
{
  return group == 0;
}

/*
    Селекторы
*/

inline instance_counter_group_iterator::reference instance_counter_group_iterator::operator * () const
{
  return *group;
}

inline instance_counter_group_iterator::pointer instance_counter_group_iterator::operator -> () const
{
  return group;
}

/*
    Инкремент / декремент
*/

inline instance_counter_group_iterator& instance_counter_group_iterator::operator ++ ()
{
  group = group->next;

  return *this;
}

inline instance_counter_group_iterator instance_counter_group_iterator::operator ++ (int)
{
  instance_counter_group_iterator res = *this;
  
  ++*this;
  
  return res;
}

/*
    Сравнение
*/

inline bool instance_counter_group_iterator::operator == (const instance_counter_group_iterator& i) const
{
  return group == i.group;
}

inline bool instance_counter_group_iterator::operator != (const instance_counter_group_iterator& i) const
{
  return !(*this == i);
}

/*
    Получение итераторов на начало и конец списка групп
*/

inline instance_counter_group_iterator get_first_instance_counter_group ()
{
  return instance_counter_group_iterator ();
}

inline instance_counter_group_iterator get_last_instance_counter_group ()
{
  return instance_counter_group_iterator (true);
}
