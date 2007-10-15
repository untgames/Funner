/*
    Интерфейс хранилища вариантных данных 
*/

struct any::holder
{
  virtual ~holder () {}

  virtual const std::type_info& type    () const = 0;
  virtual holder*               clone   () const = 0;
};

template <class T> struct any::holder_impl: public holder
{
  holder_impl (const T& in_value) : value (in_value) {}

  const std::type_info& type  () const { return typeid (T); }
  holder*               clone () const { return new holder_impl (value); }

  T value;
};

/*
    Конструкторы / деструктор
*/

inline any::any ()
  : content_ptr (0)
  {}

template <class T>
inline any::any (const T& value)
  : content_ptr (new holder_impl<T> (value))
  {}

inline any::any (const any& in_any)
  : content_ptr (in_any.content_ptr ? in_any.content_ptr->clone () : 0)
  {}

inline any::~any ()
{
  delete content_ptr;
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
  return const_cast<T*> (const_cast<const any&> (*this).content<const T> ());
}

template <class T>
inline const T* any::content () const
{
  if (!content_ptr)
    return 0;

  return &typeid (T) == &content_ptr->type () ? &static_cast<const holder_impl<T>*> (content_ptr)->value : 0;
}

/*
    Обмен
*/

inline any& any::swap (any& in_any)
{
  holder* tmp        = content_ptr;
  content_ptr        = in_any.content_ptr;
  in_any.content_ptr = tmp;
  
  return *this;
}
                       
inline void swap (any& a1, any& a2)
{
  a1.swap (a2);
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
  typedef typename remove_reference<T>::type nonref;

  nonref* result = any_cast<nonref> (&a);

  if (!result)
    throw bad_any_cast ();

  return *result;
}

template <class T>
inline const T any_cast (const any& a)
{
  typedef typename remove_reference<T>::type nonref;

  const nonref* result = any_cast<nonref> (&a);

  if (!result)
    throw bad_any_cast ();

  return *result;
}
