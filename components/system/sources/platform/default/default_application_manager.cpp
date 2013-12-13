#include "shared.h"

namespace syslib
{

namespace
{

class DefaultApplicationDelegate: public IApplicationDelegate, public xtl::reference_counter
{
  public:
///Конструктор
    DefaultApplicationDelegate ()
    {
      idle_enabled = false;
      is_exited    = false;
      listener     = 0;
    }

///Запуск цикла обработки сообщений
    void Run ()
    {
      if (listener)
        listener->OnInitialize ();
      
      while (!is_exited)
      {
        if (idle_enabled && listener)
          listener->OnIdle ();
      }
    }

///Выход из приложения
    void Exit (int code)
    {
      is_exited = true;

      if (listener)
        listener->OnExit (code);
    }

///Установка необходимости вызова событий idle
    void SetIdleState (bool state)
    {
      idle_enabled = state;
    }

///Установка слушателя событий приложения
    void SetListener (IApplicationListener* in_listener)
    {
      listener = in_listener;
    }
    
///Подсчёт ссылок
    void AddRef ()
    {
      addref (this);
    }
    
    void Release ()
    {
      release (this);
    }    

  private:
    bool                  idle_enabled;
    bool                  is_exited;
    IApplicationListener* listener;
};

}

/*
    Создание делегата приложения
*/

IApplicationDelegate* DefaultApplicationManager::CreateDefaultApplicationDelegate ()
{
  return new DefaultApplicationDelegate;
}

/*
    Приостановка выполнения нити на miliseconds милисекунд
*/

void DefaultApplicationManager::Sleep (size_t milliseconds)
{
  size_t start = common::milliseconds ();

  while (common::milliseconds () - start < milliseconds);
}

/*
    Открытие URL во внешнем браузере
*/

void DefaultApplicationManager::OpenUrl (const char* url)
{
  throw xtl::format_not_supported_exception ("syslib::DefaultApplicationManager::OpenUrl", "No OpenURL support for this platform");
}

/*
    Получение значения переменной среды
*/

stl::string DefaultApplicationManager::GetEnvironmentVariable (const char* name)
{
  const char* result = getenv (name);
  
  if (!result)
    throw xtl::format_operation_exception ("syslib::DefaultApplicationManager::GetEnvironmentVariable", "Environment variable '%s' not found");

  return result;
}

/*
    Управление энергосбережением
*/

void DefaultApplicationManager::SetScreenSaverState (bool state)
{
  throw xtl::format_not_supported_exception ("syslib::DefaultApplicationManager::SetScreenSaverState", "No screen saver support for this platform");
}

bool DefaultApplicationManager::GetScreenSaverState ()
{
  return true;
}

/*
    Управление режимом работы в фоне
*/

void DefaultApplicationManager::SetApplicationBackgroundMode (syslib::ApplicationBackgroundMode)
{
}

ApplicationBackgroundMode DefaultApplicationManager::GetApplicationBackgroundMode ()
{
  return ApplicationBackgroundMode_Active;
}

/*
    Получение системных свойств
*/

void DefaultApplicationManager::GetSystemProperties (common::PropertyMap& properties)
{
#ifdef HAS_GEEK_INFO
  try
  {
    for (int i=(int)kSystemMetricPlatform; i!=kSystemMetricCount; i++)
      properties.SetProperty (systemMetricName((SystemMetricType)i).c_str (), systemMetric((SystemMetricType)i).c_str ());
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::DefaultApplicationManager::GetSystemProperties");
    throw;
  }
#endif
}

/*
    Обработка системных сообщений нити
*/

void DefaultApplicationManager::ProcessThreadMessages ()
{
}

}
