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

#ifdef _MSC_VER  
  #if _MSC_VER < 1300
    #error Microsoft Visual C/C++ 7.0 or higher reqiured
  #endif

  //MSVC 7 typedefs wchar_t
  #if _MSC_VER < 1400
    #define NO_WCHAR
  #endif
#endif

}

#endif
