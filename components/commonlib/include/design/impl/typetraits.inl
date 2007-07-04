///////////////////////////////////////////////////////////////////////////////////////////////////
///Снятие CV-квалификатора
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T> struct clear_cv
{
  typedef T result;
  
  enum {
    is_const    = false,
    is_volatile = false
  };
};

template <class T> struct clear_cv<const T>
{
  typedef T result;
  
  enum {
    is_const    = true,
    is_volatile = false
  };
};

template <class T> struct clear_cv<volatile T>
{
  typedef T result;
  
  enum {
    is_const    = false,
    is_volatile = true
  };
};

template <class T> struct clear_cv<const volatile T>
{
  typedef T result;
  
  enum {
    is_const    = true,
    is_volatile = true
  };
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Снятие ссылки
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T> struct clear_ref
{
  typedef T result;
};

template <class T> struct clear_ref<T&>
{
  typedef T result;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Снятие указателя
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T> struct clear_ptr
{
  typedef T result;
};

template <class T> struct clear_ptr<T&>
{
  typedef T result;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Таблица конфигурации встроенных типов
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T> struct builtin_type_traits
{
  enum {
    is_integer   = false,
    is_float     = false,
    is_bool      = false,
    is_pointer   = false,
    is_reference = false,
    is_builtin   = false
  };
};

template <class T> struct builtin_type_traits<T&>
{
  enum {
    is_integer   = false,
    is_float     = false,
    is_bool      = false,
    is_pointer   = false,
    is_reference = true,
    is_builtin   = true
  };
};

template <class T> struct builtin_type_traits<T*>
{
  enum {
    is_integer   = false,
    is_float     = false,
    is_bool      = false,
    is_pointer   = true,
    is_reference = false,
    is_builtin   = true
  };
};

template <> struct builtin_type_traits<bool>
{
  enum {
    is_integer   = false,
    is_float     = false,
    is_bool      = true,
    is_pointer   = false,
    is_reference = false,
    is_builtin   = true
  };
};

struct integer_builtin_type_traits
{
  enum {
    is_integer   = true,
    is_float     = false,
    is_bool      = false,
    is_pointer   = false,
    is_reference = false,
    is_builtin   = true
  };
};

struct float_builtin_type_traits
{
  enum {
    is_integer   = false,
    is_float     = true,
    is_bool      = false,
    is_pointer   = false,
    is_reference = false,
    is_builtin   = true
  };
};

template <> struct builtin_type_traits<char>:           public integer_builtin_type_traits {};
template <> struct builtin_type_traits<signed char>:    public integer_builtin_type_traits {};
template <> struct builtin_type_traits<unsigned char>:  public integer_builtin_type_traits {};
template <> struct builtin_type_traits<short>:          public integer_builtin_type_traits {};
template <> struct builtin_type_traits<unsigned short>: public integer_builtin_type_traits {};
template <> struct builtin_type_traits<int>:            public integer_builtin_type_traits {};
template <> struct builtin_type_traits<unsigned int>:   public integer_builtin_type_traits {};
template <> struct builtin_type_traits<long>:           public integer_builtin_type_traits {};
template <> struct builtin_type_traits<unsigned long>:  public integer_builtin_type_traits {};
template <> struct builtin_type_traits<float>:          public float_builtin_type_traits {};
template <> struct builtin_type_traits<double>:         public float_builtin_type_traits {};
template <> struct builtin_type_traits<long double>:    public float_builtin_type_traits {};

#ifndef NO_WCHAR
template <> struct builtin_type_traits<wchar_t>: public integer_builtin_type_traits {};
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////
///Определение типа параметра
///////////////////////////////////////////////////////////////////////////////////////////////////

namespace detail
{

template <class T,bool test> struct parameter_type_helper
{ 
  typedef T result;
};

template <class T> struct parameter_type_helper<T,false>
{
  typedef T& result;
};

}

template <class T> struct parameter_type
{ 
  typedef typename detail::parameter_type_helper<T,builtin_type_traits<T>::is_builtin>::result result;
};
