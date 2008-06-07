/*
    Конструкторы
*/

template <class Token, class BaseIter>
inline token_iterator<Token, BaseIter>::token_iterator ()
  : first (), last ()
  {}
  
template <class Token, class BaseIter>
inline token_iterator<Token, BaseIter>::token_iterator (base_iterator in_first, base_iterator in_last)
  : first (in_first), last (in_last)
  {}

/*
    Присваивание
*/

template <class Token, class BaseIter>
inline token_iterator<Token, BaseIter>& token_iterator<Token, BaseIter>::operator = (const token_iterator& i)
{
  token_iterator (i).swap (*this);

  return *this;
}

template <class Token, class BaseIter>
inline token_iterator<Token, BaseIter>& token_iterator<Token, BaseIter>::assign (base_iterator in_first, base_iterator in_last)
{
  token_iterator (in_first, in_last).swap (*this);

  return *this;
}

/*
    Получение токена
*/

template <class Token, class BaseIter>
inline typename token_iterator<Token, BaseIter>::reference token_iterator<Token, BaseIter>::token () const
{
  return *first;
}

template <class Token, class BaseIter>
inline typename token_iterator<Token, BaseIter>::reference token_iterator<Token, BaseIter>::operator *  () const
{
  return *first;
}

template <class Token, class BaseIter>
inline typename token_iterator<Token, BaseIter>::pointer token_iterator<Token, BaseIter>::operator -> () const
{
  return &*first;
}

/*
    Количество доступных токенов / проверка на пустота
*/

template <class T>
size_t distance (T* first, T* last)
{
  return last - first;
}

template <class Token, class BaseIter>
inline typename token_iterator<Token, BaseIter>::size_type token_iterator<Token, BaseIter>::available () const
{
  return empty () ? 0 : distance (first, last);
}

template <class Token, class BaseIter>
inline bool token_iterator<Token, BaseIter>::empty () const
{
  return !(first < last);
}

template <class Token, class BaseIter>
inline token_iterator<Token, BaseIter>::operator safe_bool () const
{
  return first < last ? &token_iterator::dummy : 0;
}

/*
    Арифметика
*/

template <class Token, class BaseIter>
inline token_iterator<Token, BaseIter>& token_iterator<Token, BaseIter>::operator += (size_type step)
{
  first += step;
  return *this;  
}

template <class Token, class BaseIter>
inline token_iterator<Token, BaseIter>& token_iterator<Token, BaseIter>::operator -= (size_type step)
{
  first -= step;
  return *this;
}

template <class Token, class BaseIter>
inline token_iterator<Token, BaseIter> token_iterator<Token, BaseIter>::operator + (size_type step) const
{
  return token_iterator (*this) += step;
}

template <class Token, class BaseIter>
inline token_iterator<Token, BaseIter> token_iterator<Token, BaseIter>::operator - (size_type step) const
{
  return token_iterator (*this) -= step;
}

template <class Token, class BaseIter>
inline token_iterator<Token, BaseIter>& token_iterator<Token, BaseIter>::operator ++ ()
{
  ++first;
  return *this;
}

template <class Token, class BaseIter>
inline token_iterator<Token, BaseIter>& token_iterator<Token, BaseIter>::operator ++ (int)
{
  token_iterator tmp = *this;

  ++first;
  
  return tmp;
}

template <class Token, class BaseIter>
inline token_iterator<Token, BaseIter>& token_iterator<Token, BaseIter>::operator -- ()
{
  --first;
  return *this;
}

template <class Token, class BaseIter>
inline token_iterator<Token, BaseIter>& token_iterator<Token, BaseIter>::operator -- (int)
{
  token_iterator tmp = *this;

  --first;
  
  return tmp;
}

/*
    Обмен
*/

template <class Token, class BaseIter>
inline void token_iterator<Token, BaseIter>::swap (token_iterator& i)
{
  base_iterator tmp1 = i.first;
  i.first            = first;
  first              = tmp1;
  
  base_iterator tmp2 = i.last;
  i.last             = last;
  last               = tmp2;
}

template <class Token, class BaseIter>
inline void swap (token_iterator<Token, BaseIter>& a, token_iterator<Token, BaseIter>& b)
{
  a.swap (b);
}

/*
    Сравнение
*/

template <class Token, class BaseIter>
inline bool token_iterator<Token, BaseIter>::operator == (const token_iterator& i) const
{
  return first == i.first && last == i.last;
}

template <class Token, class BaseIter>
inline bool token_iterator<Token, BaseIter>::operator != (const token_iterator& i) const
{
  return !(*this == i);
}

/*
    Создание итератора
*/

template <class Token>
inline token_iterator<Token> make_token_iterator (Token* first, Token* last)
{
  return token_iterator<Token> (first, last);
}

template <class BaseIter>
inline token_iterator<typename iterator_traits<BaseIter>::value_type, BaseIter> make_token_iterator (BaseIter first, BaseIter last)
{
  return token_iterator<typename iterator_traits<BaseIter>::value_type, BaseIter> (first, last);
}

/*
    Чтение интервальных значений
*/

namespace detail
{

template <class Token, class BaseIter, class OutIter>
inline bool read_iter (token_iterator<Token, BaseIter>& iter, OutIter& out_iter)
{
  return read (iter, *out_iter);
}

template <class Token, class BaseIter, class T1>
inline bool read_iter (token_iterator<Token, BaseIter>& iter, T1* out_iter)
{
  return read (iter, *out_iter);
}

template <class Token, class BaseIter, class Container>
inline bool read_iter (token_iterator<Token, BaseIter>& iter, stl::back_insert_iterator<Container>& out_iter)
{
  typename Container::value_type object;

  if (!read (iter, object))
    return false;

  *out_iter = object;

  return true;
}

template <class Token, class BaseIter, class Container>
inline bool read_iter (token_iterator<Token, BaseIter>& iter, stl::front_insert_iterator<Container>& out_iter)
{
  typename Container::value_type object;

  if (!read (iter, object))
    return false;

  *out_iter = object;

  return true;
}

template <class Token, class BaseIter, class Container>
inline bool read_iter (token_iterator<Token, BaseIter>& iter, stl::insert_iterator<Container>& out_iter)
{
  typename Container::value_type object;

  if (!read (iter, object))
    return false;

  *out_iter = object;

  return true;
}

}

template <class Token, class BaseIter, class OutIter>
inline size_t read_range (token_iterator<Token, BaseIter>& iter, OutIter first, size_t count)
{
  size_t read_count = 0;
  
  for (;count-- && detail::read_iter (iter, first); ++first, ++read_count);

  return read_count;
}

template <class Token, class BaseIter, class OutIter>
inline size_t read_range (token_iterator<Token, BaseIter>& iter, OutIter first, size_t count, size_t step)
{
  if (!step)
    return read_range (iter, first, count);

  size_t                          read_count = 0;
  token_iterator<Token, BaseIter> next       = iter + step;

  for (;count-- && detail::read_iter (iter, first); ++first, iter = next, next += step, ++read_count);

  return read_count;
}

/*
    Разбор токенов
*/

namespace detail
{

template <class Token, class BaseIter, class Value>
inline bool read_value (token_iterator<Token, BaseIter>& iter, Value& value)
{
  if (!iter || !xtl::io::read (*iter, value))
    return false;

  ++iter;

  return true;
}

}

template <class BaseIter>
inline bool read (token_iterator<const char*, BaseIter>& iter, signed long& value)
{
  return detail::read_value (iter, value);
}

template <class BaseIter>
inline bool read (token_iterator<const char*, BaseIter>& iter, unsigned long& value)
{
  return detail::read_value (iter, value);
}

template <class BaseIter>
inline bool read (token_iterator<const char*, BaseIter>& iter, bool& value)
{
  return detail::read_value (iter, value);
}

template <class BaseIter>
inline bool read (token_iterator<const char*, BaseIter>& iter, wchar_t& value)
{
  return detail::read_value (iter, value);
}

template <class BaseIter>
inline bool read (token_iterator<const char*, BaseIter>& iter, char& value)
{
  return detail::read_value (iter, value);
}

template <class BaseIter>
inline bool read (token_iterator<const char*, BaseIter>& iter, signed char& value)
{
  return detail::read_value (iter, value);
}

template <class BaseIter>
inline bool read (token_iterator<const char*, BaseIter>& iter, unsigned char& value)
{
  return detail::read_value (iter, value);
}

template <class BaseIter>
inline bool read (token_iterator<const char*, BaseIter>& iter, signed short& value)
{
  return detail::read_value (iter, value);
}

template <class BaseIter>
inline bool read (token_iterator<const char*, BaseIter>& iter, unsigned short& value)
{
  return detail::read_value (iter, value);
}

template <class BaseIter>
inline bool read (token_iterator<const char*, BaseIter>& iter, signed int& value)
{
  return detail::read_value (iter, value);
}

template <class BaseIter>
inline bool read (token_iterator<const char*, BaseIter>& iter, unsigned int& value)
{
  return detail::read_value (iter, value);
}

template <class BaseIter>
inline bool read (token_iterator<const char*, BaseIter>& iter, double& value)
{
  return detail::read_value (iter, value);
}

template <class BaseIter>
inline bool read (token_iterator<const char*, BaseIter>& iter, float& value)
{
  return detail::read_value (iter, value);
}

template <class BaseIter>
inline bool read (token_iterator<const char*, BaseIter>& iter, long double& value)
{
  return detail::read_value (iter, value);
}

template <class BaseIter>
inline bool read (token_iterator<const wchar_t*, BaseIter>& iter, signed long& value)
{
  return detail::read_value (iter, value);
}

template <class BaseIter>
inline bool read (token_iterator<const wchar_t*, BaseIter>& iter, unsigned long& value)
{
  return detail::read_value (iter, value);
}

template <class BaseIter>
inline bool read (token_iterator<const wchar_t*, BaseIter>& iter, bool& value)
{
  return detail::read_value (iter, value);
}

template <class BaseIter>
inline bool read (token_iterator<const wchar_t*, BaseIter>& iter, wchar_t& value)
{
  return detail::read_value (iter, value);
}

template <class BaseIter>
inline bool read (token_iterator<const wchar_t*, BaseIter>& iter, char& value)
{
  return detail::read_value (iter, value);
}

template <class BaseIter>
inline bool read (token_iterator<const wchar_t*, BaseIter>& iter, signed char& value)
{
  return detail::read_value (iter, value);
}

template <class BaseIter>
inline bool read (token_iterator<const wchar_t*, BaseIter>& iter, unsigned char& value)
{
  return detail::read_value (iter, value);
}

template <class BaseIter>
inline bool read (token_iterator<const wchar_t*, BaseIter>& iter, signed short& value)
{
  return detail::read_value (iter, value);
}

template <class BaseIter>
inline bool read (token_iterator<const wchar_t*, BaseIter>& iter, unsigned short& value)
{
  return detail::read_value (iter, value);
}

template <class BaseIter>
inline bool read (token_iterator<const wchar_t*, BaseIter>& iter, signed int& value)
{
  return detail::read_value (iter, value);
}

template <class BaseIter>
inline bool read (token_iterator<const wchar_t*, BaseIter>& iter, unsigned int& value)
{
  return detail::read_value (iter, value);
}

template <class BaseIter>
inline bool read (token_iterator<const wchar_t*, BaseIter>& iter, double& value)
{
  return detail::read_value (iter, value);
}

template <class BaseIter>
inline bool read (token_iterator<const wchar_t*, BaseIter>& iter, float& value)
{
  return detail::read_value (iter, value);
}

template <class BaseIter>
inline bool read (token_iterator<const wchar_t*, BaseIter>& iter, long double& value)
{
  return detail::read_value (iter, value);
}

/*
    Копирование токена    
*/

template <class Token, class BaseIter>
inline bool read (token_iterator<Token, BaseIter>& iter, Token& value)
{
  if (!iter)
    return false;
    
  value = *iter;
  
  ++iter;
  
  return true;
}

/*
    Чтение STL строк
*/

template <class T, class BaseIter, class CharT, class Allocator>
inline bool read (token_iterator<const T*, BaseIter>& iter, stl::basic_string<T, CharT, Allocator>& s)
{
  if (!iter)
    return false;

  s = *iter;
  
  ++iter;

  return true;
}

/*
    Чтение токенов с приведением типов
*/

template <class Base, class Token, class BaseIter, class Value>
inline bool read_and_cast (token_iterator<Token, BaseIter>& iter, Value& value)
{
  Base base_value;

  if (!read (iter, base_value))
    return false;

  value = static_cast<Value> (base_value);

  return true;  
}

/*
    Чтение токенов с возвратом значения
*/

template <class Value, class Token, class BaseIter>
inline Value get (token_iterator<Token, BaseIter>& iter)
{
  Value result;
  
  if (read (iter, result))
    return result;
    
  throw bad_lexical_cast (typeid (token_iterator<Token, BaseIter>), typeid (Value));
}

template <class Value, class Token, class BaseIter>
inline Value get (token_iterator<Token, BaseIter>& iter, const Value& default_value)
{
  Value result;
  
  return read (iter, result) ? result : default_value;
}

/*
    Чтение токенов
*/

template <class Token, class BaseIter, class Value>
inline void read (token_iterator<Token, BaseIter>& iter, Value& value, const Value& default_value)
{
  if (!read (iter, value))
    value = default_value;
}
