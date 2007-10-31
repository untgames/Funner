#ifndef XTL_DYNAMIC_CAST_ROOT_HEADER
#define XTL_DYNAMIC_CAST_ROOT_HEADER

#include <xtl/default_cast_type.h>

namespace stl
{

//forward declarations
template <class T> class auto_ptr;

}

namespace xtl
{

//forward declarations
template <class T>                 class shared_ptr;
template <class T, class Strategy> class intrusive_ptr;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Корень для автоматических приведений через dynamic_cast
///////////////////////////////////////////////////////////////////////////////////////////////////
class dynamic_cast_root
{
  protected:
            dynamic_cast_root  () {}
    virtual ~dynamic_cast_root () {}
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Если тип не приводим к dynamic_cast_root - возвращаем 0
///////////////////////////////////////////////////////////////////////////////////////////////////
inline dynamic_cast_root* get_root (default_cast_type)
{
  return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение dynamic_cast_root
///////////////////////////////////////////////////////////////////////////////////////////////////
dynamic_cast_root*       get_root (dynamic_cast_root&);
dynamic_cast_root*       get_root (dynamic_cast_root*);
const dynamic_cast_root* get_root (const dynamic_cast_root&);
const dynamic_cast_root* get_root (const dynamic_cast_root*);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение dynamic_cast_root для интеллектуальных указателей
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T>                       dynamic_cast_root* get_root (const shared_ptr<T>&);
template <class T, class Strategy>       dynamic_cast_root* get_root (const intrusive_ptr<T, Strategy>&);
template <class T>                       dynamic_cast_root* get_root (const stl::auto_ptr<T>&);
template <class T>                 const dynamic_cast_root* get_root (const shared_ptr<const T>&);
template <class T, class Strategy> const dynamic_cast_root* get_root (const intrusive_ptr<const T, Strategy>&);
template <class T>                 const dynamic_cast_root* get_root (const stl::auto_ptr<const T>&);

#include <xtl/detail/dynamic_cast_root.inl>

}

#endif
