#ifndef SYSLIB_IPHONE_PLATFORM_SHARED_HEADER
#define SYSLIB_IPHONE_PLATFORM_SHARED_HEADER

#include <ctime>

#include <CoreFoundation/CFRunLoop.h>

#include <stl/algorithm>
#include <stl/vector>

#include <xtl/common_exceptions.h>
#include <xtl/function.h>

#include <common/component.h>
#include <common/file.h>
#include <common/log.h>
#include <common/strlib.h>
#include <common/xml_writer.h>

#include <syslib/application_delegate.h>
#include <syslib/timer.h>

#include <syslib/platform/iphone.h>

#include <shared/platform.h>

namespace syslib
{

bool is_in_run_loop (); //запущен ли главный цикл

}

#endif
