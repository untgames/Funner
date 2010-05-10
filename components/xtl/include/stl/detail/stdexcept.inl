/*
    Базовый класс именованого исключения
*/

inline named_exception::named_exception (const char* s)
{
  strncpy (name,s?s:"unknown exception",BUF_SIZE);
  name [BUF_SIZE-1] = '\0';  
}

inline const char* named_exception::what () const throw ()
{
  return name;
}

/*
    Логические ошибки
*/

inline logic_error::logic_error (const char* s) 
  : named_exception (s?s:"logic_error") 
  { }
  
inline domain_error::domain_error (const char* s)
  : logic_error (s?s:"domain_error") 
  { }
  
inline invalid_argument::invalid_argument (const char* s)
  : logic_error (s?s:"invalid_argument") 
  { }
  
inline length_error::length_error (const char* s)
  : logic_error (s?s:"length_error") 
  { }  
  
inline out_of_range::out_of_range (const char* s)
  : logic_error (s?s:"out_of_range") 
  { }    
 
/*
    Ошибки времени выполнения
*/

inline runtime_error::runtime_error (const char* s)
  : named_exception (s?s:"runtime_error") 
  { }    
  
inline range_error::range_error (const char* s)
  : runtime_error (s?s:"range_error") 
  { }

inline overflow_error::overflow_error (const char* s)
  : runtime_error (s?s:"overflow_error") 
  { }
  
inline underflow_error::underflow_error (const char* s)
  : runtime_error (s?s:"underflow_error") 
  { }  

/*
    Возбуждение исключения
*/  

template <class Exception>
void raise (const char* format,...)
{
  char buf [named_exception::BUF_SIZE];
  
  va_list  lst;
  
  va_start  (lst,format);
  
#ifndef _MSC_VER  
  vsnprintf (buf,sizeof (buf)-1,format,lst);
#else
  _vsnprintf (buf,sizeof (buf)-1,format,lst);
#endif

  buf [sizeof (buf)-1] = '\0';
  
  throw Exception (buf);
}

template <class Container>
inline char const* stl_get_container_name (const Container&)
{
  return typeid (Container).name ();
}

template <class Container>
inline void stl_raise_length_error (const Container& c,size_t new_size)
{
  raise<length_error> ("%s: Container is too long (new_size=%u, current_size=%u, max_size=%u)",
                       stl_get_container_name (c),new_size,c.size (),c.max_size ());
}

template <class Container>
inline void stl_raise_out_of_range (const Container& c,int position)
{
  raise<out_of_range> ("%s: Invalid position %d in container with %u elements",
                       stl_get_container_name (c),position,c.size ());
}

template <class Container>
inline void stl_raise_out_of_range (const Container& c,int first,int last)
{
  raise<out_of_range> ("%s: Invalid range [%d;%d) in container with %u elements",
                       stl_get_container_name (c),first,last,c.size ());
}

template <class Container>
inline void stl_raise_out_of_range (const Container& c,typename Container::iterator i)
{
  stl_raise_out_of_range (c,distance (typename Container::const_iterator (i),c.begin ()));
}

template <class Container>
inline void stl_raise_out_of_range 
 (const Container&             c,
  typename Container::iterator first,
  typename Container::iterator last)
{
  stl_raise_out_of_range (c,distance (typename Container::const_iterator (first),c.begin ()),
                            distance (typename Container::const_iterator (last),c.begin ()));
}
