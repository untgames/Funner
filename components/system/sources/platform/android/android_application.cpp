#include "shared.h"

using namespace syslib;

namespace
{

/// Делегат приложения
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

IApplicationDelegate* Platform::CreateDefaultApplicationDelegate ()
{
  return new DefaultApplicationDelegate;
}
