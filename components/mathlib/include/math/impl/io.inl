/*
    Чтение математических типов
*/

template <class Token, class BaseIter, class T, size_t size>
inline bool read (xtl::io::token_iterator<Token, BaseIter>& iter, vec<T, size>& value)
{
  if (iter.available () < size)
    return false;

  xtl::io::token_iterator<Token, BaseIter> save = iter;
    
  for (size_t i=0; i<size; i++) 
    if (!read (iter, value [i]))
    {
      iter = save;
      return false;
    }

  return true;
}

template <class Token, class BaseIter, class T, size_t size>
inline bool read (xtl::io::token_iterator<Token, BaseIter>& iter, matrix<T, size>& value)
{
  if (iter.available () < size * size)
    return false;

  xtl::io::token_iterator<Token, BaseIter> save = iter;

  for (size_t i=0; i<size; i++) 
    for (size_t j=0; j<size; j++)   
      if (!read (iter, value [i][j]))
      {
        iter = save;
        return false;
      }

  return true;  
}

template <class Token, class BaseIter, class T>
inline bool read (xtl::io::token_iterator<Token, BaseIter>& iter, quat<T>& value)
{
  if (iter.available () < 4)
    return false;

  xtl::io::token_iterator<Token, BaseIter> save = iter;
    
  for (size_t i=0; i<4; i++) 
    if (!read (iter, value [i]))
    {
      iter = save;
      return false;
    }

  return true;
}

/*
    Сериализация математических типов
*/

namespace detail
{

template <class T, size_t size, class Separator>
inline void write (common::OutputTextStream& stream, const vec<T,size>& value, const char* format, const Separator& separator)
{
  for (size_t i=0; i<size-1; i++)
  {
    write (stream, value [i], format);
    write (stream, separator);
  }
  
  write (stream, value [size-1], format);
}

template <class T, size_t size, class Separator>
inline void write (common::OutputTextStream& stream, const matrix<T,size>& value, const char* format, const Separator& separator)
{
  const T* array = &value [0][0];

  for (size_t i=0; i<size*size-1; i++)
  {
    write (stream, array [i], format);
    write (stream, separator);
  }
  
  write (stream, array [size*size-1], format);
}

template <class T, class Separator>
inline void write (common::OutputTextStream& stream, const quat<T>& value, const char* format, const Separator& separator)
{
  for (size_t i=0; i<3; i++)
  {
    write (stream, value [i], format);
    write (stream, separator);
  }
  
  write (stream, value [3], format);
}

}

template <class T, size_t size>
inline void write (common::OutputTextStream& stream, const vec<T,size>& value, const char* format, const char* separator)
{
  detail::write (stream, value, format, separator);
}

template <class T, size_t size>
inline void write (common::OutputTextStream& stream, const vec<T,size>& value, const char* format, const wchar_t* separator)
{
  detail::write (stream, value, format, separator);
}

template <class T, size_t size>
inline void write (common::OutputTextStream& stream, const matrix<T,size>& value, const char* format, const char* separator)
{
  detail::write (stream, value, format, separator);
}

template <class T, size_t size>
inline void write (common::OutputTextStream& stream, const matrix<T,size>& value, const char* format, const wchar_t* separator)
{
  detail::write (stream, value, format, separator);
}

template <class T>
inline void write (common::OutputTextStream& stream, const quat<T>& value, const char* format, const char* separator)
{
  detail::write (stream, value, format, separator);
}

template <class T>
inline void write (common::OutputTextStream& stream, const quat<T>& value, const char* format, const wchar_t* separator)
{
  detail::write (stream, value, format, separator);
}
