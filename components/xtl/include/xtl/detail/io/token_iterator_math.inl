/*
    Чтение математических типов
*/

namespace detail
{

template <class Token, class BaseIter, class T>
bool read_math_value (token_iterator<Token, BaseIter>& iter, T& value)
{
  return read (iter, value);
}

template <class Token, class BaseIter>
bool read_math_value (token_iterator<Token, BaseIter>& iter, unsigned char& value)
{
  return read_and_cast<unsigned int> (iter, value);
}

template <class Token, class BaseIter>
bool read_math_value (token_iterator<Token, BaseIter>& iter, signed char& value)
{
  return read_and_cast<int> (iter, value);
}

template <class Token, class BaseIter>
bool read_math_value (token_iterator<Token, BaseIter>& iter, char& value)
{
  return read_and_cast<int> (iter, value);
}

}

template <class Token, class BaseIter, class T, unsigned int Size>
inline bool read (token_iterator<Token, BaseIter>& iter, math::vector<T, Size>& value)
{
  if (iter.available () < Size)
    return false;

  token_iterator<Token, BaseIter> save = iter;
    
  for (unsigned int i=0; i<Size; i++)
    if (!detail::read_math_value (iter, value [i]))
    {
      iter = save;
      return false;
    }

  return true;
}

template <class Token, class BaseIter, class T, unsigned int Size>
inline bool read (token_iterator<Token, BaseIter>& iter, math::matrix<T, Size>& value)
{
  if (iter.available () < Size * Size)
    return false;

  token_iterator<Token, BaseIter> save = iter;

  for (unsigned int i=0; i<Size; i++) 
    for (unsigned int j=0; j<Size; j++)   
      if (!detail::read_math_value (iter, value [i][j]))
      {
        iter = save;
        return false;
      }

  return true;  
}

template <class Token, class BaseIter, class T>
inline bool read (token_iterator<Token, BaseIter>& iter, math::quat<T>& value)
{
  if (iter.available () < 4)
    return false;

  token_iterator<Token, BaseIter> save = iter;
    
  for (unsigned int i=0; i<4; i++) 
    if (!detail::read_math_value (iter, value [i]))
    {
      iter = save;
      return false;
    }

  return true;
}
