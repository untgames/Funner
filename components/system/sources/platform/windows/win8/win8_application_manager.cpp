#include "shared.h"

using namespace syslib;

namespace
{

/// Фабрика создания основного окна приложения
ref class ApplicationSource sealed: Windows::ApplicationModel::Core::IFrameworkViewSource
{
  public:
    virtual Windows::ApplicationModel::Core::IFrameworkView^ CreateView ()
    {
printf ("%s(%u)\n", __FUNCTION__, __LINE__); fflush (stdout);
for (;;);
      return nullptr;
    }
};

/// Делегат приложения
class Win8ApplicationDelegate: public IApplicationDelegate, public xtl::reference_counter
{
  public:
///Конструктор
    Win8ApplicationDelegate ()
    {
      idle_enabled    = false;
      is_exited       = false;
      listener        = 0;
    }

///Запуск цикла обработки сообщений
    void Run ()
    {
      try
      {
        if (listener)
          listener->OnInitialize ();

	auto source = ref new ApplicationSource;

	Windows::ApplicationModel::Core::CoreApplication::Run (source);
      }
      catch (Platform::Exception^ e)
      {
        throw xtl::format_operation_exception ("syslib::Win8ApplicationDelegate::Run", "Platform exception %s. %s", tostring (e->Message).c_str (), _com_error (e->HResult).ErrorMessage ());
      }
      catch (xtl::exception& e)
      {
        e.touch ("syslib::Win8ApplicationDelegate::Run");
        throw;
      }
    }

///Выход из приложения
    void Exit (int code)
    {
//      if (!PostThreadMessage (main_thread_id, WM_QUIT, code, 0))
//        raise_error ("::PostThreadMessage");
    }

///Установка необходимости вызова событий idle
    void SetIdleState (bool state)
    {
      idle_enabled = state;
      
      if (state)
      {
//        if (!::PostThreadMessage (main_thread_id ? main_thread_id : GetCurrentThreadId (), wake_up_message, 0, 0))
//          raise_error ("::PostThreadMessage");
      }
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

IApplicationDelegate* WindowsApplicationManager::CreateDefaultApplicationDelegate ()
{
  return new Win8ApplicationDelegate;
}

/*
   Открытие URL во внешнем браузере
*/

void WindowsApplicationManager::OpenUrl (const char* url)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

/*
    Получение значения переменной среды
*/

#undef GetEnvironmentVariable

stl::string WindowsApplicationManager::GetEnvironmentVariable (const char* name)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

/*
    Управление энергосбережением
*/

void WindowsApplicationManager::SetScreenSaverState (bool state)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

bool WindowsApplicationManager::GetScreenSaverState ()
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}
