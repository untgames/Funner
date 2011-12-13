#include <ctime>

#include <errno.h>
#include <dlfcn.h>

#include <bps/bps.h>
#include <bps/navigator.h>
#include <screen/screen.h>
#include <bps/screen.h>
#include <bps/bps.h>
#include <bps/event.h>

#include <stl/hash_map>

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


class IWindowImpl
{
  public:
    virtual void OnWindowEvent (int event_type, screen_event_t event) = 0;
};

class WindowRegistry
{
  public:
    static void RegisterWindow   (screen_window_t window, IWindowImpl* impl);
    static void UnregisterWindow (screen_window_t window);    

    static IWindowImpl* FindWindow (screen_window_t window);
};

}

}
