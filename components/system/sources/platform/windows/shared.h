
#include <stl/hash_map>
#include <stl/hash_set>

#include <xtl/common_exceptions.h>
#include <xtl/intrusive_ptr.h>
#include <xtl/reference_counter.h>

#include <common/component.h>
#include <common/file.h>
#include <common/log.h>
#include <common/property_map.h>
#include <common/singleton.h>
#include <common/strlib.h>
#include <common/utf_converter.h>
#include <common/xml_writer.h>

#include <syslib/application.h>

#ifdef LoadLibrary
#undef LoadLibrary
#endif

#include <shared/platform.h>

#include <process.h>

#define _WIN32_WINNT 0x0600 //для использования WM_MOUSEWHEEL
#include <windows.h>

#ifdef WINCE
#else
#include <Wtsapi32.h>

WINGDIAPI  HGDIOBJ   WINAPI SelectObject(HDC,HGDIOBJ);

#endif

namespace syslib
{

//проверка ошибок использования WinAPI и генерация исключения в случае их наличия
void check_errors (const char* source);
void raise_error  (const char* source);

//очистка tls нити
void cleanup_tls ();

}
