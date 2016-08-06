/*
    any
*/

namespace detail
{

namespace adl_defaults
{

/*
    Свободные функции по умолчанию, для игнорирования отсутствия соответствующих псевдонимов при ADL
*/

using xtl::to_string;

struct cast_type
{
  template <class T> cast_type (const T&) : info (&typeid (T)) {}

  const std::type_info* info;
};

//по умолчанию типы не преобразуются к строкам
inline void to_string (stl::string& s, cast_type type)
{
  s += type.info->name ();
}

}

/*
    Интерфейс хранилища вариантных данных
*/

struct any_holder: public reference_counter
{
  custom_ref_caster     caster;
  const std::type_info* castable_type;
  const bool            is_self_changable;

  template <class T>
  any_holder (T* content, bool in_is_self_changable)
    : caster (content)
    , castable_type (content ? &typeid (*content) : &typeid (void))
    , is_self_changable (in_is_self_changable)
  {}

  virtual ~any_holder () {}

  virtual any_holder*           clone                 () = 0;
  virtual const std::type_info& type                  () = 0;
  virtual void                  dump                  (stl::string&) = 0;
  virtual void                  update_castable_value () = 0;
};

/*
    Объект, хранящий значение (используется для корректной работы с неполными ссылочными типами)
*/

#ifdef _MSC_VER
  #pragma warning (push)
  #pragma warning (disable:4624) //destructor could not be generated because a base class destructor is inaccessible
#endif

/*
    Содержимое вариативной переменной
*/

template <class T> struct any_content
{
  any_content (T& in_value) : value (in_value)  {}

  T value;
};

template <class T> struct any_impl: public any_content<T>, public any_holder
{
  typedef any_content<T> content;

  any_impl (T& value) : content (value), any_holder (get_castable_value (content::value), any_value_self_changable<typename type_traits::remove_cv<T>::type>::value != 0) {}
  
  const std::type_info& type () { return typeid (T); }

  void update_castable_value ()
  {
    caster = get_castable_value (content::value);
    castable_type = &typeid (*get_castable_value (content::value));
  }

  any_holder* clone () { return new any_impl<T> (*this); }

  void dump (stl::string& buffer)
  {
    using adl_defaults::to_string;

    to_string (buffer, *get_castable_value (content::value));
  }
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
  : content_ptr (new detail::any_impl<T> (const_cast<T&> (value))) //fix it
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
  return !content_ptr || content_ptr->caster.empty ();
}

/*
    Тип и данные
*/

inline const std::type_info& any::type () const
{
  return content_ptr ? content_ptr->type () : typeid (void);
}

inline const std::type_info& any::castable_type () const
{
  return content_ptr ? *content_ptr->castable_type : typeid (void);
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

  return &static_cast<detail::any_impl<T>*> (const_cast<detail::any_holder*> (content_ptr))->value;
}

/*
    Приведение
*/

template <class T>
inline const T any::cast () const
{
  if (!content_ptr)
    throw bad_any_cast (type (), typeid (T));

  if (!content_ptr->is_self_changable)
    return content_ptr->caster.cast<T> ();

  content_ptr->update_castable_value ();

  return content_ptr->caster.cast<T> ();
}

/*
    Печать в строку
*/

inline void any::to_string (stl::string& buffer) const
{
  if (!content_ptr)
  {
    buffer.clear ();
    return;
  }

  content_ptr->dump (buffer);
}

inline void to_string (stl::string& buffer, const volatile any& value)
{
  const_cast<const any&> (value).to_string (buffer);
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
inline T* get_castable_value (T& value)
{
  return &value;
}

template <class T>
inline T* get_castable_value (T* value)
{
  return value;
}

template <class T>
inline T* get_castable_value (stl::auto_ptr<T>& ptr)
{
  return ptr.get();
}

template <class T>
inline T* get_castable_value (shared_ptr<T>& ptr)
{
  return ptr.get();
}

template <class T>
inline T* get_castable_value (weak_ptr<T>& ptr)
{
  return ptr.get();
}

template <class T, template <class > class Strategy>
inline T* get_castable_value (intrusive_ptr<T, Strategy>& ptr)
{
  return ptr.get();
}

template <class T>
inline T* get_castable_value (com_ptr<T>& ptr)
{
  return ptr.get();
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

inline void** get_castable_value (void*& ptr)
{
  return &ptr;
}

inline const char** get_castable_value (const char*& ptr)
{
  return &ptr;
}
