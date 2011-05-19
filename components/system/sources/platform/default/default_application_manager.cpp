#include "shared.h"

using namespace syslib;

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
        listener->OnInitialized ();
      
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
