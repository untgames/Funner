#include <process.h>

#include <stl/hash_map>
#include <stl/hash_set>

#include <xtl/common_exceptions.h>
#include <xtl/intrusive_ptr.h>
#include <xtl/reference_counter.h>

#include <common/file.h>
#include <common/singleton.h>
#include <common/strlib.h>

#include <syslib/application.h>

#include <platform/platform.h>

#define _WIN32_WINNT 0x0600 //для использования WM_MOUSEWHEEL
#include <windows.h>

namespace syslib
{

//проверка ошибок использования WinAPI и генерация исключения в случае их наличия
void check_errors (const char* source);
void raise_error  (const char* source);

//очистка tls нити
void cleanup_tls ();

}
