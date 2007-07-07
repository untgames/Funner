/*

    Конструктор и присваивание
*/

template <class WordT>
inline bit_reference<WordT>::bit_reference (word_type* _pos,word_type _mask)
  : pos (_pos), mask (_mask)
  { }

template <class WordT>
inline bit_reference<WordT>& bit_reference<WordT>::operator = (bool value)
{
  if (value) *pos |= mask;
  else       *pos &= ~mask;

  return *this;
}

template <class WordT>
inline bit_reference<WordT>& bit_reference<WordT>::operator = (const bit_reference& ref)
{
  if (ref) *pos |= mask;
  else     *pos &= ~mask;

  return *this;
}

/*
    Двоичные операции
*/
 
template <class WordT>
bit_reference<WordT>& bit_reference<WordT>::flip ()
{
  *pos ^= mask;
  return *this;
}

template <class WordT>
inline bool bit_reference<WordT>::operator ~ () const
{
  return ((*pos) & mask) == 0;
}

template <class WordT>
inline bit_reference<WordT>& bit_reference<WordT>::operator |= (bool value)
{
  if (value)
      *pos |= mask;
      
  return *this;  
}

template <class WordT>
inline bit_reference<WordT>& bit_reference<WordT>::operator &= (bool value)
{
  if (!value)
    *pos &= ~mask;
      
  return *this;  
}

template <class WordT>
inline bool bit_reference<WordT>::operator | (bool value) const
{
  return bool (*this) || value;
}

template <class WordT>
inline bool bit_reference<WordT>::operator & (bool value) const
{
  return bool (*this) && value;
}

/*
    Преобразование к bool
*/

template <class WordT>
inline bit_reference<WordT>::operator bool () const
{
  return ((*pos) & mask) != 0;
}

/*
    Обмен
*/

template <class WordT>
inline void bit_reference<WordT>::swap (bit_reference& x)
{
  bool tmp = *this;
  *this    = x;
  x        = tmp;
}

template <class WordT>
inline void swap (bit_reference<WordT>& x,bit_reference<WordT>& y)
{
  x.swap (y);
}

/*
    Сравнение
*/

template <class WordT>
inline bool bit_reference<WordT>::operator == (const bit_reference& x) const
{
  return bool (*this) == bool (x);
}

template <class WordT>
inline bool bit_reference<WordT>::operator < (const bit_reference& x) const
{
  return !bool (*this) && bool (x);
}

template <class WordT>
inline bool bit_reference<WordT>::operator != (const bit_reference& x) const
{
  return !(*this == x);
}

template <class WordT>
inline bool bit_reference<WordT>::operator > (const bit_reference& x) const
{
  return x < *this;
}

template <class WordT>
inline bool bit_reference<WordT>::operator <= (const bit_reference& x) const
{
  return !(x < *this);
}

template <class WordT>
inline bool bit_reference<WordT>::operator >= (const bit_reference& x) const
{
  return !(*this < x);
}
