#ifndef SYSLIB_UNISTD_PLATFORM_HEADER
#define SYSLIB_UNISTD_PLATFORM_HEADER

#include <shared/default_window_manager.h>
#include <shared/default_library_manager.h>
#include <shared/default_timer_manager.h>
#include <shared/default_application_manager.h>
#include <shared/default_screen_keyboard_manager.h>
#include <shared/pthread_manager.h>

namespace syslib
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Менеджер библиотек POSIX
///////////////////////////////////////////////////////////////////////////////////////////////////
class UnistdLibraryManager: public DefaultLibraryManager
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Загрузка динамических библиотек
///////////////////////////////////////////////////////////////////////////////////////////////////
    static dll_t LoadLibrary   (const wchar_t* name);
    static void  UnloadLibrary (dll_t);
    static void* GetSymbol     (dll_t, const char* symbol_name);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Менеджер таймеров Unistd
///////////////////////////////////////////////////////////////////////////////////////////////////
class UnistdTimerManager: public DefaultTimerManager
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Создание / уничтожение таймера
///////////////////////////////////////////////////////////////////////////////////////////////////
    static timer_t CreateTimer (size_t period_in_milliseconds, TimerHandler, void* user_data);
    static void    KillTimer   (timer_t handle);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Менеджер приложения POSIX
///////////////////////////////////////////////////////////////////////////////////////////////////
class UnistdApplicationManager: public DefaultApplicationManager
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Приостановка выполнения нити на miliseconds милисекунд
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void Sleep (size_t miliseconds);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Работа с очередью сообщений
///////////////////////////////////////////////////////////////////////////////////////////////////
    static IApplicationDelegate* CreateDefaultApplicationDelegate ();
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Платформа
///////////////////////////////////////////////////////////////////////////////////////////////////
class UnistdPlatform
 : public DefaultWindowManager
 , public UnistdTimerManager 
 , public PThreadManager 
 , public UnistdLibraryManager
 , public UnistdApplicationManager
 , public DefaultScreenManager 
 , public DefaultSensorManager 
 , public DefaultCookieManager
 , public DefaultScreenKeyboardManager
{
};

}

#endif
