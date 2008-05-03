#ifndef MEDIALIB_CLONE_HEADER
#define MEDIALIB_CLONE_HEADER

namespace xtl
{

//forward declarations
template <class T> class com_ptr;

template <class T1, class T2> com_ptr<T1> static_pointer_cast (const com_ptr<T2>&);
  
}

namespace media
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///–ежим копировани€
///////////////////////////////////////////////////////////////////////////////////////////////////
enum CloneMode
{
  CloneMode_Copy,     //копирование
  CloneMode_Instance, //инстанцирование (увеличение числа ссылок)
};

///////////////////////////////////////////////////////////////////////////////////////////////////
/// опирование
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T> inline T clone (T& resource, CloneMode mode = CloneMode_Copy)
{
  return T (resource, mode);
}

template <class T> inline xtl::com_ptr<T> clone (xtl::com_ptr<T>& ptr, CloneMode mode = CloneMode_Copy)
{
  return ptr ? xtl::static_pointer_cast<T> (ptr->Clone (mode)) : 0;
}

}

#endif
