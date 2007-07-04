inline size_t hash (int x)
{
  return x;
}

inline size_t hash (unsigned int x)
{
  return x;
}

inline size_t hash (const char* s)
{
  size_t h = 0; 
  
  while (*s) h = 5 * h + *s++;
  
  return h;
}

inline size_t hash (const unsigned char* s)
{
  return hash ((const char*)s);
}

inline size_t hash (const wchar_t* s)
{
  size_t h = 0; 
  
  while (*s) h = 5 * h + *s++;
  
  return h;
}

template <class T> 
size_t hash (const T* p)
{
  return (size_t)p;
}
