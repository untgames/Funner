#include <ctime>

#include <errno.h>
#include <dlfcn.h>

#include <bps/bps.h>
#include <bps/navigator.h>
#include <screen/screen.h>

#include <xtl/common_exceptions.h>
#include <xtl/reference_counter.h>
#include <xtl/string.h>

#include <common/singleton.h>
#include <common/time.h>
#include <common/utf_converter.h>

#include <shared/platform.h>

namespace syslib
{

namespace tabletos
{

//инициализация платформы
void platform_initialize ();

//проверка ошибок использования BPS и генерация исключения в случае их наличия
void check_errors (const char* source);
void raise_error  (const char* source);

}

}
