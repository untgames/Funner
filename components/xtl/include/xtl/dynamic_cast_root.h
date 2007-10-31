#ifndef XTL_DYNAMIC_CAST_ROOT_HEADER
#define XTL_DYNAMIC_CAST_ROOT_HEADER

namespace xtl
{

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
///Получение dynamic_cast_root
///////////////////////////////////////////////////////////////////////////////////////////////////
dynamic_cast_root*       get_root (dynamic_cast_root&);
dynamic_cast_root*       get_root (dynamic_cast_root*);
const dynamic_cast_root* get_root (const dynamic_cast_root&);
const dynamic_cast_root* get_root (const dynamic_cast_root*);

#include <xtl/detail/dynamic_cast_root.inl>

}

#endif
