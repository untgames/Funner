/*
    any
*/

namespace detail
{

/*
    Проверка на 0
*/

template <class T>
inline bool is_null (const T& value)
{
  return &value == 0;
}

template <class T>
inline bool is_null (const T* value)
{
  return value == 0;
}

/*
    Интерфейс хранилища вариантных данных
*/

struct any_holder: public reference_counter
{
  virtual ~any_holder () {}

  virtual any_holder*           clone      () = 0;
  virtual const std::type_info& type       () = 0;
  virtual bool                  null       () = 0;
  virtual custom_ref_caster     get_caster () = 0;
};

/*
    Объект, хранящий значение (испрользуется для корректной работы с неполными ссылочными типами)
*/

#ifdef _MSC_VER
  #pragma warning (push)
  #pragma warning (disable:4624) //destructor could not be generated because a base class destructor is inaccessible
#endif

/*
    Содержимое вариативной переменной
*/

template <class T> struct any_content: public any_holder
{
  any_content (const T& in_value) : value (const_cast<T&> (in_value))  {}
  
  const std::type_info& type () { return typeid (T); }
  bool                  null () { return is_null (value); }

  any_holder* clone () { return new any_content<T> (*this); }

  custom_ref_caster get_caster ()
  {
    return custom_ref_caster (get_castable_value (value));
  }

  T value;
};

}

/*
===================================================================================================
    any
===================================================================================================
*/

/*
    Конструкторы / деструктор
*/

inline any::any ()
  : content_ptr (0)
  {}

template <class T>
inline any::any (const T& value)
  : content_ptr (new detail::any_content<T> (value))
  {}
  
inline any::any (detail::any_holder* in_content_ptr)
  : content_ptr (in_content_ptr)
  {}  

inline any::any (const any& in_any)
  : content_ptr (in_any.content_ptr)
{
  if (content_ptr)
    addref (content_ptr);
}

inline any::~any ()
{
  if (content_ptr)
    release (content_ptr);
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
    Копирование
*/

inline any any::clone () const
{
  return content_ptr ? any (content_ptr->clone ()) : any ();
}

/*
    Проверка на пустоту / проверка на 0
*/

inline bool any::empty () const
{
  return content_ptr == 0;
}

inline bool any::null () const
{
  return !content_ptr || content_ptr->null ();
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

  if (&typeid (T) != &content_ptr->type ())
    return 0; //преобразование невозможно, из-за неэквивалентности базовых типов

  return &static_cast<detail::any_content<T>*> (const_cast<detail::any_holder*> (content_ptr))->value;
}

/*
    Приведение
*/

namespace detail
{

template <class T> struct any_return_value_wrapper
{
  T value;
  
  any_return_value_wrapper () : value () {}
};

}

template <class T>
inline const T any::cast () const
{
  if (!content_ptr)
    throw bad_any_cast (type (), typeid (T));

  return content_ptr->get_caster ().cast<T> ();
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
    Утилиты
*/

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
    throw bad_any_cast (a.type (), typeid (T));

  return *result;
}

template <class T>
inline const T any_cast (const any& a)
{
  typedef typename type_traits::remove_reference<T>::type nonref;

  const nonref* result = any_cast<nonref> (&a);

  if (!result)
    throw bad_any_cast (a.type (), typeid (T));

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

/*
    Получение приводимого значения. Используется как базовое при работе any_multicast
*/

template <class T>
inline T& get_castable_value (T& value)
{
  return value;
}

template <class T>
inline T* get_castable_value (T* ptr)
{
  return ptr;
}

inline char* get_castable_value (char* ptr)
{
  return ptr;
}

inline wchar_t* get_castable_value (wchar_t* ptr)
{
  return ptr;
}

template <class T>
inline T* get_castable_value (stl::auto_ptr<T>& ptr)
{
  return &*ptr;
}

template <class T>
inline T* get_castable_value (shared_ptr<T>& ptr)
{
  return &*ptr;
}

template <class T, template <class > class Strategy>
inline T* get_castable_value (intrusive_ptr<T, Strategy>& ptr)
{
  return &*ptr;
}

template <class T>
inline T* get_castable_value (com_ptr<T>& ptr)
{
  return &*ptr;
}

template <class T>
inline T* get_castable_value (reference_wrapper<T>& ref)
{
  return &ref.get ();
}

template <class T>
inline T* get_castable_value (trackable_ptr<T>& ptr)
{
  return &*ptr;
}
