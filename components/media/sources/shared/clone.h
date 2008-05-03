#ifndef MEDIALIB_SHARED_CLONE_HEADER
#define MEDIALIB_SHARED_CLONE_HEADER

#include <common/exception.h>
#include <media/clone.h>

namespace xtl
{

//forward declaration
template <class T, template <class> class Strategy> class intrusive_ptr;

}

namespace media
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Копирование ресурса
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T, template <class> class Strategy>
inline xtl::intrusive_ptr<T, Strategy> clone (const xtl::intrusive_ptr<T, Strategy>& ptr, CloneMode mode, const char* source="")
{
  if (!source || !*source)
    source = "media::clone";

  if (!ptr)
    common::RaiseNullArgument (source, "ptr");
    
  switch (mode)
  {
    case CloneMode_Instance: return ptr;
    case CloneMode_Copy:     return xtl::intrusive_ptr<T, Strategy> (new T (*ptr), false);
    default:                 common::RaiseInvalidArgument (source, "mode", mode);
  }
  
  return 0;
}

}

#endif
