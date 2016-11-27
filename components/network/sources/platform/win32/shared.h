#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>

#include <xtl/common_exceptions.h>
#include <xtl/reference_counter.h>

#include <common/log.h>
#include <common/singleton.h>
#include <common/strlib.h>

#include <platform/platform.h>

namespace network
{

//Инициализация работы с сетью
void init_networking ();

//проверка ошибок использования WinAPI и генерация исключения в случае их наличия
void check_errors (const char* source);
void raise_error  (const char* source);

//получение строки с сообщением об ошибке
stl::string get_error_message (DWORD error_code);

}
