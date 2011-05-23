#ifndef __MYSTL_CHAR_TRAITS__
#define __MYSTL_CHAR_TRAITS__

#include <string.h>
#include <wchar.h> //cwchar имеет проблемы компиляции на mingw 3.4.5

#include <stl/functional>

namespace stl
{

////////////////////////////////////////////////////////////////////////////////////////
///Базовые cимвольные операции
////////////////////////////////////////////////////////////////////////////////////////
template <class Char,class Int>
struct char_traits_base
{
  typedef Char char_type;
  typedef Int  int_type;

////////////////////////////////////////////////////////////////////////////////////////
///Присваивание
////////////////////////////////////////////////////////////////////////////////////////
  static void       assign (char_type& dst,const char_type& src);
  static char_type* assign (char_type* dst,size_t count,const char_type& ch);
  
////////////////////////////////////////////////////////////////////////////////////////
///Быстрая инициализация
////////////////////////////////////////////////////////////////////////////////////////  
  static void fast_init (char_type* dst,size_t count); //по умолчанию инициализация Char ()
  
////////////////////////////////////////////////////////////////////////////////////////
///Копирование
////////////////////////////////////////////////////////////////////////////////////////
  static char_type* copy (char_type* dst,const char_type* src,size_t count);
  static char_type* move (char_type* dst,const char_type* src,size_t count);
  
////////////////////////////////////////////////////////////////////////////////////////
///Сравнение сомволов и строк
////////////////////////////////////////////////////////////////////////////////////////  
  static bool eq          (const char_type&,const char_type&);
  static bool eq_int_type (const int_type&,const int_type&);  
  static bool lt          (const char_type&,const char_type&);      //less 
  static int  compare     (const char_type*,const char_type*,size_t count);

////////////////////////////////////////////////////////////////////////////////////////
///Поиск символа / длина строки
////////////////////////////////////////////////////////////////////////////////////////      
  static const char_type* find   (const char_type*,size_t count,const char_type& ch);
  static size_t           length (const char_type*);  

////////////////////////////////////////////////////////////////////////////////////////
///Преобразования    
////////////////////////////////////////////////////////////////////////////////////////
  static const char_type to_char_type (const int_type&);
  static const int_type  to_int_type  (const char_type&);
  
////////////////////////////////////////////////////////////////////////////////////////
///Маркер EOF / NULL
////////////////////////////////////////////////////////////////////////////////////////      
  static const int_type  eof     ();
  static const int_type  not_eof (const int_type&);
  static const char_type null    ();
};

template <class Char> struct char_traits: public char_traits_base<Char,Char> {};

////////////////////////////////////////////////////////////////////////////////////////
///Специализация символьных операций char
////////////////////////////////////////////////////////////////////////////////////////
template <> 
struct char_traits<char>: public char_traits_base<char,int>
{ 
  static char*       copy        (char*,const char*,size_t);
  static char*       move        (char*,const char*,size_t);
  static char*       assign      (char*,size_t,const char&);  
  static void        assign      (char&,char);
  static void        fast_init   (char* dst,size_t count) {} //инициализация отсутствует
  static int         compare     (const char*,const char*,size_t); 
  static size_t      length      (const char*);
  static const char* find        (const char*,size_t count,const char&);
  static int         to_int_type (const char&);
};

////////////////////////////////////////////////////////////////////////////////////////
///Специализация символьных операций wchar_t
////////////////////////////////////////////////////////////////////////////////////////
template <>
struct char_traits<wchar_t>: public char_traits_base<wchar_t,wint_t>
{
  static void     fast_init (wchar_t* dst,size_t count) {} //инициализация отсутствует
  static wchar_t* copy      (wchar_t*,const wchar_t*,size_t);
  static wchar_t* move      (wchar_t*,const wchar_t*,size_t);
  static size_t   length    (const wchar_t*);
};

////////////////////////////////////////////////////////////////////////////////////////
///Функтор проверки равенства символов
////////////////////////////////////////////////////////////////////////////////////////
template <class Traits>
struct eq_traits: public binary_function<typename Traits::char_type,typename Traits::char_type,bool>
{
  typedef typename Traits::char_type char_type;

  bool operator () (const char_type& x,const char_type& y) const { return Traits::eq (x,y); }
};

////////////////////////////////////////////////////////////////////////////////////////
///Функтор проверки порядка следования символов
////////////////////////////////////////////////////////////////////////////////////////
template <class Traits>
struct lt_traits: public binary_function<typename Traits::char_type,typename Traits::char_type,bool>
{
  typedef typename Traits::char_type char_type;

  bool operator () (const char_type& x,const char_type& y) const { return Traits::lt (x,y); }
};

#include <stl/detail/char_traits.inl>

}

#endif 
