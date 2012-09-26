#ifndef STORE_COMMON_HEADER
#define STORE_COMMON_HEADER

#include <xtl/functional_fwd>

namespace store
{

typedef xtl::function <void (const void*)> ReleaseHandleFunction;

}

#endif
