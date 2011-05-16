#ifndef BOUND_VOLUMES_PLANE_LIST_HEADER
#define BOUND_VOLUMES_PLANE_LIST_HEADER

#ifdef _MSC_VER
  #pragma push_macros(MATH_BOUND_VOLUMES_PLANE_LIST_NO_DETAILS)
#endif

#define  MATH_BOUND_VOLUMES_PLANE_LIST_NO_DETAILS
#include <bv/basic_plane_list.h>
#undef   MATH_BOUND_VOLUMES_PLANE_LIST_NO_DETAILS

#ifdef _MSC_VER
  #pragma pop_macros
#endif

namespace bound_volumes
{

template plane_listf;
template plane_listd;

}

#endif
