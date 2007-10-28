/*
    Описание контейнера any
*/

namespace detail
{

namespace adl_defaults
{

/*
    Утилиты    
*/

using xtl::get_root;
using xtl::to_string;
using xtl::to_value;

//получение корня приведения по умолчанию
inline dynamic_cast_root* get_root (default_cast_type)
{
  return 0;
}

//по умолчанию типы не преобразуются к строкам
inline void to_string (stl::string&, default_cast_type)
{
  throw bad_any_cast ();
}

//по умолчанию типы не приводимы из строки
inline void to_value (const stl::string& buffer, default_cast_type)
{
  throw bad_any_cast ();
}

}

/*
    Интерфейс хранилища вариантных данных 
*/

struct any_holder
{
  virtual ~any_holder () {}

  virtual const std::type_info& type    () = 0;
  virtual any_holder*           clone   () = 0;
  virtual void                  release () = 0;

  virtual dynamic_cast_root* get_dynamic_cast_root () = 0;
  virtual void               dump (stl::string&) = 0;
};

/*
    Содержимое вариативной переменной
*/

template <class T> struct any_content: public any_holder
{
  any_content (const T& in_value) : value (in_value) {}

  const std::type_info& type () { return typeid (T); }
  
  dynamic_cast_root* get_dynamic_cast_root ()
  {
    using adl_defaults::get_root;
  
    return get_root (value);
  }
  
  void dump (stl::string& buffer)
  {
    using adl_defaults::to_string;
  
    to_string (buffer, value);
  }

  T value;
};

/*
    Реализации вариативной переменной
*/

template <class T> struct copyable_any_impl: public any_content<T>
{
  copyable_any_impl (const T& in_value) : any_content<T> (in_value) {}

  any_holder* clone   () { return new copyable_any_impl (*this); }
  void        release () { delete this; }
};

template <class T> class referenced_any_impl: public any_content<T>
{
  public:
    referenced_any_impl (const T& in_value) : any_content<T> (in_value), ref_count (1) {}

  any_holder* clone () { return ref_count++, this; }
  
  void release ()
  {
    if (!--ref_count)
      delete this;
  }

  private:
    size_t ref_count;
};

}

/*
    any
*/

/*
    Конструкторы / деструктор
*/

inline any::any ()
  : content_ptr (0)
  {}

template <class T>
inline any::any (const T& value, bool ref_counted)
  : content_ptr (create_holder (value, ref_counted))
  {}

inline any::any (const any& in_any)
  : content_ptr (in_any.content_ptr ? in_any.content_ptr->clone () : 0)
  {}

inline any::~any ()
{
  if (content_ptr)
    content_ptr->release ();
}

/*
    Создание хранилища
*/

template <class T>
inline detail::any_holder* any::create_holder (const T& value, bool ref_counted)
{
  return ref_counted ? static_cast<detail::any_holder*> (new detail::referenced_any_impl<T> (value)) :
                       static_cast<detail::any_holder*> (new detail::copyable_any_impl<T> (value));
}

/*
    Присваивание
*/

inline any& any::operator = (const any& in_any)
{
  any (in_any).swap (*this);
  return *this;
}

template <class T>
inline any& any::operator = (const T& value)
{
  any (value).swap (*this);
  return *this;
}

/*
    Проверка на пустоту
*/

inline bool any::empty () const
{
  return content_ptr == 0;
}

/*
    Тип и данные
*/

inline const std::type_info& any::type () const
{
  return content_ptr ? content_ptr->type () : typeid (void);
}

template <class T>
inline T* any::content ()
{
  return const_cast<T*> (const_cast<const any&> (*this).content<T> ());
}

template <class T>
inline const T* any::content () const
{
  if (!content_ptr)
    return 0;

  return &typeid (T) == &content_ptr->type () ? &static_cast<const detail::any_content<T>*> (content_ptr)->value : 0;
}

/*
    Приведение
*/

namespace detail
{

//обёртки для приведения типов через dynamic_cast
template <class DstT, bool has_vtbl=type_traits::is_polymorphic<DstT>::value>
struct dynamic_caster_helper
{
  static DstT* cast (dynamic_cast_root* source)
  {
    DstT* result = dynamic_cast<DstT*> (source);
    
    if (!result)
      throw bad_any_cast ();
      
    return result;
  }
};

template <class DstT> struct dynamic_caster_helper<DstT, false>
{
  static DstT* cast (dynamic_cast_root*) { throw bad_any_cast (); }
};

template <class DstT> struct dynamic_caster
{
  static DstT& cast (dynamic_cast_root* source) { return *dynamic_caster_helper<DstT>::cast (source); }
};

template <class DstT> struct dynamic_caster<DstT*>
{
  static DstT* cast (dynamic_cast_root* source) { return dynamic_caster_helper<DstT>::cast (source); }
};

//попытка приведения строки в значение
template <class T>
inline T try_lexical_cast (const stl::string& buffer)
{
  using adl_defaults::to_value;

  T value;

  to_value (buffer, value);

  return value;
}

}

template <class T>
inline const T any::cast () const
{
  if (!content_ptr)
    throw bad_any_cast ();

  typedef typename type_traits::remove_reference<T>::type nonref;
  
    //попытка прямого приведения
  
  nonref* result = const_cast<nonref*> (content<nonref> ());
  
  if (result)
    return *result;

    //попытка приведения через dynamic_cast_root
  
  if (dynamic_cast_root* dc_root = content_ptr->get_dynamic_cast_root ())
    return detail::dynamic_caster<nonref>::cast (dc_root);

    //попытка лексикографического приведения

  stl::string buffer;

  content_ptr->dump (buffer);

  return detail::try_lexical_cast<nonref> (buffer);
}

/*
    Обмен
*/

inline any& any::swap (any& in_any)
{
  detail::any_holder* tmp = content_ptr;
  content_ptr             = in_any.content_ptr;
  in_any.content_ptr      = tmp;
  
  return *this;
}
                       
inline void swap (any& a1, any& a2)
{
  a1.swap (a2);
}

/*
    Создание any с подсчётом ссылок
*/

template <class T>
inline any make_ref_any (const T& value)
{
  return any (value, true);
}

/*
    Приведение типов
*/

template <class T>
inline T* any_cast (any* a)
{
  return a ? a->content<T> () : 0;
}

template <class T>
inline const T* any_cast (const any* a)
{
  return a ? a->content<const T> () : 0;
}

template <class T>
inline T any_cast (any& a)
{
  typedef typename type_traits::remove_reference<T>::type nonref;

  nonref* result = any_cast<nonref> (&a);

  if (!result)
    throw bad_any_cast ();

  return *result;
}

template <class T>
inline const T any_cast (const any& a)
{
  typedef typename type_traits::remove_reference<T>::type nonref;

  const nonref* result = any_cast<nonref> (&a);

  if (!result)
    throw bad_any_cast ();

  return *result;
}

/*
    Многоуровневое приведение типов
*/

template <class T>
inline const T any_multicast (const any& a)
{
  return a.cast<T> ();
}
