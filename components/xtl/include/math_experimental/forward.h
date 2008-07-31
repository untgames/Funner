#ifndef MATHLIB_FORWARD_HEADER
#define MATHLIB_FORWARD_HEADER
#include <stddef.h>

#ifdef _MSC_VER  
  #pragma pack(push,1)
#endif


namespace math
{
  template <class Type, size_t Size> class vec;
  template <class Type, size_t SizeX, size_t SizeY=SizeX> class matrix;
  template <class T> class quat;
  template <class T> class point;
  template <class T> class plane;
 

#ifdef _MSC_VER
  #pragma pack (pop)
#endif
}

#endif
