#ifndef __MYSTL_TYPE_TRAITS__
#define __MYSTL_TYPE_TRAITS__

#include <cstddef>

namespace stl
{

struct false_type {};
struct true_type {};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Таблица конфигурации типов  
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T> struct type_traits
{
  typedef false_type has_trivial_default_constructor;
  typedef false_type has_trivial_copy_constructor;
  typedef false_type has_trivial_assignment_operator;
  typedef false_type has_trivial_destructor;
  typedef false_type is_POD_type;
  typedef false_type is_integer;
  typedef false_type is_float;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Специализация для встроенных типов
///////////////////////////////////////////////////////////////////////////////////////////////////
struct PODT_traits
{
  typedef true_type  has_trivial_default_constructor;
  typedef true_type  has_trivial_copy_constructor;
  typedef true_type  has_trivial_assignment_operator;
  typedef true_type  has_trivial_destructor;
  typedef true_type  is_POD_type;
  typedef false_type is_integer;
  typedef false_type is_float;  
};

struct integer_traits: public PODT_traits
{
  typedef true_type is_integer;  
};

struct float_traits: public PODT_traits
{
  typedef true_type is_float; 
};

template <> struct type_traits<bool>:           public PODT_traits {};
template <> struct type_traits<char>:           public integer_traits {};
template <> struct type_traits<signed char>:    public integer_traits {};
template <> struct type_traits<unsigned char>:  public integer_traits {};
template <> struct type_traits<short>:          public integer_traits {};
template <> struct type_traits<unsigned short>: public integer_traits {};
template <> struct type_traits<int>:            public integer_traits {};
template <> struct type_traits<unsigned int>:   public integer_traits {};
template <> struct type_traits<long>:           public integer_traits {};
template <> struct type_traits<unsigned long>:  public integer_traits {};
template <> struct type_traits<float>:          public float_traits {};
template <> struct type_traits<double>:         public float_traits {};
template <> struct type_traits<long double>:    public float_traits {};

#ifndef NO_WCHAR
template <> struct type_traits<wchar_t>:        public integer_traits {};
#endif

template <class T> struct type_traits<T*>:      public PODT_traits {};

}

#endif
