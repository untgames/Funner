#ifndef __MYSTL_TEMP_BUF__
#define __MYSTL_TEMP_BUF__

#include <stl/utility>
#include <stl/alloc.h>

namespace stl
{

/////////////////////////////////////////////////////////////////////////
///Временный буфер
/////////////////////////////////////////////////////////////////////////    
template <class T>
class temporary_buffer
{
  public:
    typedef T                  value_type;
    typedef value_type         *pointer;
    typedef const value_type   *const_pointer;
    typedef value_type&        reference;
    typedef const value_type&  const_reference;
    typedef size_t             size_type;
    typedef ptrdiff_t          difference_type;
    
    temporary_buffer  (size_type request_size);
    temporary_buffer  (size_type request_size,const T& value);
    ~temporary_buffer ();
    
/////////////////////////////////////////////////////////////////////////
///Размер / запрошенный размер 
/////////////////////////////////////////////////////////////////////////
    size_type size () const;
    size_type requested_size () const;
    
/////////////////////////////////////////////////////////////////////////
///Получение итераторов
/////////////////////////////////////////////////////////////////////////    
    pointer       begin  ();
    pointer       end    ();
    const_pointer begin  () const;
    const_pointer end    () const;
    
  private:
    temporary_buffer (const temporary_buffer&) {}
    void operator =  (const temporary_buffer&) {}   
    
  private:
    size_type requested_len, len;
    pointer   buffer;  
};

/////////////////////////////////////////////////////////////////////////
///Выделение памяти для временных операций
/////////////////////////////////////////////////////////////////////////    
template <class T> 
pair<T*,size_t> get_temporary_buffer (size_t len);

template <class T> 
void return_temporary_buffer (T* buffer);

#include <stl/detail/tempbuf.inl>

}

#endif
