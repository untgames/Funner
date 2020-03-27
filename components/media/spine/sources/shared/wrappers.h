#ifndef MEDIALIB_SPINE_SHARED_WRAPPERS_HEADER
#define MEDIALIB_SPINE_SHARED_WRAPPERS_HEADER

#include <typeinfo>

#include <xtl/common_exceptions.h>

namespace media
{

namespace spine
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Wrappers for objects creation
///////////////////////////////////////////////////////////////////////////////////////////////////
class Wrappers
{
  public:
    template <class Ret, class T>
    static Ret* Unwrap (const T& object)
    {
      return object.impl;
    }

    template <class Ret, class T>
    static Ret Wrap (T* ptr)
    {
      if (!ptr)
        throw xtl::format_operation_exception ("media::spine::Wrappers::Wrap", "Attempt to wrap null internal object of type '%s'", typeid (T).name ());

      return Ret (ptr);
    }

    template <class Ret, class T>
    static Ret* WrapToNew (T* ptr)
    {
      if (!ptr)
        throw xtl::format_operation_exception ("media::spine::Wrappers::WrapToNew", "Attempt to wrap null internal object of type '%s'", typeid (T).name ());

      return new Ret (ptr);
    }
};

}

}

#endif
