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
