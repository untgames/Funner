#include <platform/platform.h>
#include <common/exception.h>
#include <common/singleton.h>
#include <common/strlib.h>
#include <stl/hash_map>

#define _WIN32_WINNT 0x0501 //для использования WM_MOUSEWHEEL
#include <windows.h>

namespace syslib
{

//исключение: ошибка использования WinAPI
struct WinAPIExceptionTag;

typedef common::DerivedException<common::Exception, WinAPIExceptionTag> WinAPIException;

//проверка ошибок использования WinAPI и генерация исключения в случае их наличия
void check_errors (const char* source);
void raise_error  (const char* source);

}
