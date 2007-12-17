#ifndef __MYSTL_CONFIG__
#define __MYSTL_CONFIG__

namespace stl
{

//forward declarations
template <class T> class allocator;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Аллокатор по умолчанию (может быть специализирован пользователем)
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T> struct default_allocator        { typedef allocator<T> allocator_type; };
template <class T> struct default_string_allocator { typedef allocator<T> allocator_type; };

}

#endif
