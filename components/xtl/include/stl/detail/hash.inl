/*
    Ключ хэширования
*/

template <class T>
inline hash_key<T>::hash_key (const value_type& value)
  : hash (hash_fun<T> ()(value))
  {}

template <class T>
inline size_t hash_key<T>::get_hash () const
{
  return hash;
}

template <class T>
inline bool hash_key<T>::operator == (const hash_key& key) const
{
  return hash == key.hash;
}

template <class T>
inline bool hash_key<T>::operator != (const hash_key& key) const
{
  return hash != key.hash;
}

/*
    Функции хэширования
*/

inline size_t hash (int x)
{
  return x;
}

inline size_t hash (unsigned int x)
{
  return x;
}

template <class T> inline size_t hash (const T* p)
{
  return (size_t)p;
}

template <class T> inline size_t hash (const hash_key<T>& key)
{
  return key.get_hash ();
}

inline size_t hash (const unsigned char* s)
{
  return hash ((const char*)s);
}

#ifndef __MYSTL_STANDALONE__

inline size_t hash (const char* s)
{
  return common::strhash (s);
}

inline size_t hash (const wchar_t* s)
{
  return common::strhash (s);
}

#else

inline size_t hash (const char* s)
{
  size_t h = 0; 
  
  while (*s) h = 5 * h + *s++;
  
  return h;
}

inline size_t hash (const wchar_t* s)
{
  size_t h = 0; 
  
  while (*s) h = 5 * h + *s++;
  
  return h;
}

#endif
