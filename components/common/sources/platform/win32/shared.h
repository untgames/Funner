#ifndef COMMONLIB_PLATFORM_WIN32_SHARED_HEADER
#define COMMONLIB_PLATFORM_WIN32_SHARED_HEADER

#include <windows.h>

#include <xtl/common_exceptions.h>

#include <common/strlib.h>

#include <platform/platform.h>

namespace common
{

//проверка ошибок использования WinAPI и генерация исключения в случае их наличия
void check_errors (const char* source);
void raise_error  (const char* source);

}

#endif
