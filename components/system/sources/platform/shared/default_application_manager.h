#ifndef SYSLIB_DEFAULT_APPLICATION_MANAGER_HEADER
#define SYSLIB_DEFAULT_APPLICATION_MANAGER_HEADER

#include <stl/string>

#include <syslib/application.h>

namespace common
{

//forward declaration
class PropertyMap;

}

namespace syslib
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Менеджер приложения по умолчанию
///////////////////////////////////////////////////////////////////////////////////////////////////
class DefaultApplicationManager
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Приостановка выполнения нити на miliseconds милисекунд
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void Sleep (size_t miliseconds);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Открытие URL во внешнем браузере
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void OpenUrl (const char* url);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение значения переменной окружения
///////////////////////////////////////////////////////////////////////////////////////////////////
    static stl::string GetEnvironmentVariable (const char* name);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Работа с очередью сообщений
///////////////////////////////////////////////////////////////////////////////////////////////////
    static IApplicationDelegate* CreateDefaultApplicationDelegate ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Управление энергосбережением
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void SetScreenSaverState (bool state);
    static bool GetScreenSaverState ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Управление режимом работы в фоне
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void                              SetApplicationBackgroundMode (syslib::ApplicationBackgroundMode mode);
    static syslib::ApplicationBackgroundMode GetApplicationBackgroundMode ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение системных свойств
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void GetSystemProperties (common::PropertyMap&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Обработка системных сообщений нити
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void ProcessThreadMessages ();
};

}

#endif
