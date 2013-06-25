#ifndef XTL_TYPE_INFO_HEADER
#define XTL_TYPE_INFO_HEADER

#include <cstddef>
#include <typeinfo>

#include <xtl/any_cast_exception.h>
#include <xtl/functional_traits.h>
#include <xtl/singleton_default.h>
#include <xtl/type.h>
#include <xtl/type_info_decl.h>
#include <xtl/type_traits>
#include <xtl/type_list.h>
#include <xtl/type_info_decl.h>

namespace xtl
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение информации о типе
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T> const type_info& get_type ();
template <class T> const type_info& get_type (const T&);
template <class T> const type_info& get_type (T&);

#include <xtl/detail/type_info.inl>

}

#endif
