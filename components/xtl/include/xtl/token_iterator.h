#ifndef XTL_TOKEN_ITERATOR_HEADER
#define XTL_TOKEN_ITERATOR_HEADER

#include <cstddef>
#include <xtl/token_parser.h>

namespace stl
{

//forward declarations
template <class Iter>      struct iterator_traits;
template <class Container> class  back_insert_iterator;
template <class Container> class  front_insert_iterator;
template <class Container> class  insert_iterator;
                           struct random_access_iterator_tag;

template <class T, class CharT, class Allocator> class basic_string;

}

namespace math
{

//forward declarations
template <class T, unsigned int Size> class vector;
template <class T, unsigned int Size> class matrix;
template <class T>                    class quat;

}

namespace xtl
{

namespace io
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Таблица свойств итератора
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class Iter> struct iterator_traits: public stl::iterator_traits<Iter> {};

//специализация для указателей
template <class T> struct iterator_traits<T*>
{
  typedef stl::random_access_iterator_tag iterator_category;
  typedef T*                              pointer;
  typedef T&                              reference;
  typedef size_t                          size_type;
  typedef ptrdiff_t                       difference_type;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Итератор для чтения токенов
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class Token, class BaseIter=Token*>
class token_iterator
{
  typedef void (token_iterator::*safe_bool)();
  typedef BaseIter base_iterator;
  typedef iterator_traits<base_iterator> base_iterator_traits;
  public:
    typedef typename base_iterator_traits::size_type                  size_type;
    typedef typename base_iterator_traits::difference_type            difference_type;
    typedef Token                                                     value_type;
    typedef typename base_iterator_traits::reference                  reference;
    typedef typename base_iterator_traits::pointer                    pointer;
    typedef token_iterator                                            iterator;
    typedef token_iterator<const value_type>                          const_iterator;
    typedef typename iterator_traits<base_iterator>::iterator_category iterator_category;
  
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструкторы
///////////////////////////////////////////////////////////////////////////////////////////////////
    token_iterator ();
    token_iterator (base_iterator first, base_iterator last);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Присваивание
///////////////////////////////////////////////////////////////////////////////////////////////////
    token_iterator& operator = (const token_iterator&);
    token_iterator& assign     (base_iterator first, base_iterator last);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение токена
///////////////////////////////////////////////////////////////////////////////////////////////////
    reference token       () const;
    reference operator *  () const;
    pointer   operator -> () const;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Количество доступных токенов / проверка на пустоту
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_type available () const;    
    bool      empty     () const;
    operator  safe_bool () const { return first < last ? &token_iterator::dummy : 0; }
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Арифметика
///////////////////////////////////////////////////////////////////////////////////////////////////
    token_iterator& operator += (size_type);
    token_iterator& operator -= (size_type);
    token_iterator  operator  + (size_type) const;
    token_iterator  operator  - (size_type) const;
    token_iterator& operator ++ ();
    token_iterator& operator ++ (int);
    token_iterator& operator -- ();
    token_iterator& operator -- (int);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Обмен
///////////////////////////////////////////////////////////////////////////////////////////////////
    void swap (token_iterator&);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Сравнение
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool operator == (const token_iterator&) const;
    bool operator != (const token_iterator&) const;
    
  private:
    void dummy () {}

  private:
    base_iterator first, last;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Обмен
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class Token, class BaseIter>
void swap (token_iterator<Token, BaseIter>&, token_iterator<Token, BaseIter>&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Создание итератора
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class Token>
token_iterator<Token> make_token_iterator (Token* first, Token* last);

template <class BaseIter>
token_iterator<typename iterator_traits<BaseIter>::value_type, BaseIter> make_token_iterator (BaseIter first, BaseIter last);

//////////////////////////////////////////////////////////////////////////////////////////////////
///Копирование токена
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class Token, class BaseIter>
bool read (token_iterator<Token, BaseIter>&, Token& result);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Разбор токенов базовых типов
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class BaseIter> bool read (token_iterator<const char*, BaseIter>&, bool&);
template <class BaseIter> bool read (token_iterator<const char*, BaseIter>&, char&);
template <class BaseIter> bool read (token_iterator<const char*, BaseIter>&, unsigned char&);
template <class BaseIter> bool read (token_iterator<const char*, BaseIter>&, signed short&);
template <class BaseIter> bool read (token_iterator<const char*, BaseIter>&, unsigned short&);
template <class BaseIter> bool read (token_iterator<const char*, BaseIter>&, signed int&);
template <class BaseIter> bool read (token_iterator<const char*, BaseIter>&, unsigned int&);
template <class BaseIter> bool read (token_iterator<const char*, BaseIter>&, signed long&);
template <class BaseIter> bool read (token_iterator<const char*, BaseIter>&, unsigned long&);
template <class BaseIter> bool read (token_iterator<const char*, BaseIter>&, float&);
template <class BaseIter> bool read (token_iterator<const char*, BaseIter>&, double&);
template <class BaseIter> bool read (token_iterator<const char*, BaseIter>&, long double&);
template <class BaseIter> bool read (token_iterator<const wchar_t*, BaseIter>&, bool&);
template <class BaseIter> bool read (token_iterator<const wchar_t*, BaseIter>&, char&);
template <class BaseIter> bool read (token_iterator<const wchar_t*, BaseIter>&, unsigned char&);
template <class BaseIter> bool read (token_iterator<const wchar_t*, BaseIter>&, signed short&);
template <class BaseIter> bool read (token_iterator<const wchar_t*, BaseIter>&, unsigned short&);
template <class BaseIter> bool read (token_iterator<const wchar_t*, BaseIter>&, signed int&);
template <class BaseIter> bool read (token_iterator<const wchar_t*, BaseIter>&, unsigned int&);
template <class BaseIter> bool read (token_iterator<const wchar_t*, BaseIter>&, signed long&);
template <class BaseIter> bool read (token_iterator<const wchar_t*, BaseIter>&, unsigned long&);
template <class BaseIter> bool read (token_iterator<const wchar_t*, BaseIter>&, float&);
template <class BaseIter> bool read (token_iterator<const wchar_t*, BaseIter>&, double&);
template <class BaseIter> bool read (token_iterator<const wchar_t*, BaseIter>&, long double&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Чтение STL строк
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T, class BaseIter, class CharT, class Allocator>
bool read (token_iterator<const T*, BaseIter>&, stl::basic_string<T, CharT, Allocator>&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Чтение токенов с приведением типов
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class Base, class Token, class BaseIter, class Value>
bool read_and_cast (token_iterator<Token, BaseIter>&, Value& value);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Чтение интервалов
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class Token, class BaseIter, class OutIter>
size_t read_range (token_iterator<Token, BaseIter>&, OutIter first, size_t count = ~0);

template <class Token, class BaseIter, class OutIter>
size_t read_range (token_iterator<Token, BaseIter>&, OutIter first, size_t count, size_t step);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Чтение токенов с возвратом значения
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class Value, class Token, class BaseIter> Value get (token_iterator<Token, BaseIter>&); //throws bad_lexical_cast
template <class Value, class Token, class BaseIter> Value get (token_iterator<Token, BaseIter>&, const Value& default_value); //nothrow

///////////////////////////////////////////////////////////////////////////////////////////////////
///Чтение токенов
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class Token, class BaseIter, class Value>
void read (token_iterator<Token, BaseIter>&, Value& value, const Value& default_value);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Чтение математических типов
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class Token, class BaseIter, class T, unsigned int Size>
bool read (token_iterator<Token, BaseIter>&, math::vector<T, Size>&);

template <class Token, class BaseIter, class T, unsigned int Size>
bool read (token_iterator<Token, BaseIter>&, math::matrix<T, Size>&);

template <class Token, class BaseIter, class T>
bool read (token_iterator<Token, BaseIter>&, math::quat<T>&);

#include <xtl/detail/io/token_iterator.inl>
#include <xtl/detail/io/token_iterator_math.inl>

}

}

#endif
