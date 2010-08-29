#include <winsock2.h>
#include <windows.h>

#include <xtl/common_exceptions.h>

#include <common/singleton.h>
#include <common/strlib.h>

#include <platform/platform.h>

namespace network
{

//проверка ошибок использования WinAPI и генерация исключения в случае их наличия
void check_errors (const char* source);
void raise_error  (const char* source);

}
