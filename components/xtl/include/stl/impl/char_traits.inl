/*
    Присваивание
*/

template <class Char,class Int>
inline void char_traits_base<Char,Int>::assign (char_type& dst,const char_type& src)
{
  dst = src;
}

template <class Char,class Int>
inline typename char_traits_base<Char,Int>::char_type* char_traits_base<Char,Int>::assign 
( char_type*       dst,
  size_t           count,
  const char_type& c)
{
  char_type* res = dst;
    
  while (count--) *dst++ = c;

  return res;
}

/*
    Быстрая инициализация
*/

template <class Char,class Int>
inline void char_traits_base<Char,Int>::fast_init (char_type* dst,size_t count)
{
  assign (dst,count,null ());
}

/*
    Копирование
*/

template <class Char,class Int>
inline typename char_traits_base<Char,Int>::char_type* char_traits_base<Char,Int>::copy 
( char_type*       dst,
  const char_type* src,
  size_t          count)
{
  char_type* res = dst;
  
  while (count--) *dst++ = *src++;
  
  return res;
}

template <class Char,class Int>
inline typename char_traits_base<Char,Int>::char_type* char_traits_base<Char,Int>::move 
( char_type*       dst,
  const char_type* src,
  size_t           count)
{
  char_type* res = dst;

  if (src < dst && dst < src + count)
  { 
    for (src+=count,dst+=count;count--;*--dst=*--src); //overlapped
  }  
  else while (count--) *dst++ = *src++;
         
  return res;
}

/*
    Сравнение сомволов и строк
*/
    
template <class Char,class Int>
inline bool char_traits_base<Char,Int>::eq (const char_type& a,const char_type& b)
{
  return a == b;
}

template <class Char,class Int>
inline bool char_traits_base<Char,Int>::lt (const char_type& a,const char_type& b)
{
  return a < b;
}

template <class Char,class Int>
inline bool char_traits_base<Char,Int>::eq_int_type (const int_type& a,const int_type& b)
{
  return a == b; 
}

template <class Char,class Int>
inline int char_traits_base<Char,Int>::compare (const char_type* a,const char_type* b,size_t count)
{
  for (;count--;a++,b++)
    if (!eq (*a,*b))
      return lt (*a,*b) ? -1 : 1;

  return 0;
}

/*
    Поиск символа / длина строки
*/

template <class Char,class Int>
inline const typename char_traits_base<Char,Int>::char_type* char_traits_base<Char,Int>::find
( const char_type* s,
  size_t           count,
  const char_type& c)
{
  for (;count--;s++)
    if (eq (*s,c))
      return s;
      
  return 0;
}

template <class Char,class Int>
inline size_t char_traits_base<Char,Int>::length (const char_type* s)
{
  size_t    count = 0;
  char_type c     = null ();
  
  while (!eq (*s++,c)) count++;
  
  return count;
}

/*
    Преобразования
*/

template <class Char,class Int>
inline const typename char_traits_base<Char,Int>::char_type char_traits_base<Char,Int>::to_char_type (const int_type& x)
{
  return static_cast<char_type> (x);
}

template <class Char,class Int>
inline const typename char_traits_base<Char,Int>::int_type char_traits_base<Char,Int>::to_int_type (const char_type& x)
{
  return static_cast<int_type> (x);
} 

/*
    Маркеры
*/

template <class Char,class Int>
inline const typename char_traits_base<Char,Int>::int_type char_traits_base<Char,Int>::eof ()
{
  return static_cast<int_type> (-1);
}

template <class Char,class Int>
inline const typename char_traits_base<Char,Int>::char_type char_traits_base<Char,Int>::null ()
{
  return char_type ();
}

template <class Char,class Int>
inline const typename char_traits_base<Char,Int>::int_type char_traits_base<Char,Int>::not_eof (const int_type& x)
{
  return !eq_int_type (x,eof ()) ? x : 0;
}

/*
    Специализация для char
*/

inline char* char_traits<char>::copy (char* dst,const char* src,size_t count)
{
  return (char*)memcpy (dst,src,count);
}

inline char* char_traits<char>::move (char* dst,const char* src,size_t count)
{
  return (char*)memmove (dst,src,count);
}

inline void char_traits<char>::assign (char& dst,char src)
{
  dst = src;
}

inline char* char_traits<char>::assign (char* dst,size_t count,const char& c)
{
  return (char*)memset (dst,c,count);
}

inline int char_traits<char>::compare (const char* a,const char* b,size_t count)
{
  return memcmp (a,b,count);
}

inline size_t char_traits<char>::length (const char* a)
{
  return strlen (a);
}

inline const char* char_traits<char>::find (const char* s,size_t count,const char& c)
{
  return (const char*)memchr (s,c,count);
}

inline int char_traits<char>::to_int_type (const char& c)
{
  return (unsigned char)c;
}

/*
    Специализация для wchar_t
*/

inline wchar_t* char_traits<wchar_t>::copy (wchar_t* dst,const wchar_t* src,size_t count)
{
  return (wchar_t*)memcpy (dst,src,count*sizeof (wchar_t));
}

inline wchar_t* char_traits<wchar_t>::move (wchar_t* dst,const wchar_t* src,size_t count)
{
  return (wchar_t*)memmove (dst,src,count*sizeof (wchar_t));
}

inline size_t char_traits<wchar_t>::length (const wchar_t* a)
{
  return wcslen (a);
}
