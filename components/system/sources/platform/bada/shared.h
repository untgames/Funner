#ifndef SYSTEMLIB_BADA_SHARED_HEADER
#define SYSTEMLIB_BADA_SHARED_HEADER

#include <xtl/common_exceptions.h>

#include <common/singleton.h>

#include <syslib/platform/bada.h>

#include <platform/platform.h>

#include <FBase.h>
#include <FApp.h>
#include <FUi.h>

namespace syslib
{

namespace bada
{

void raise (const char* source, result r);

}

}

#endif
