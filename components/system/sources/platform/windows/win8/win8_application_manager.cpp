#include "shared.h"

using namespace Windows::UI::Core;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::ApplicationModel::Activation;
using namespace Windows::Foundation;

using namespace syslib;

namespace
{

ref class ApplicationView;

ApplicationView^ app_view;

/// Основное окно приложения
ref class ApplicationView sealed: Windows::ApplicationModel::Core::IFrameworkView, public IApplicationContext
{
  public:
    // This method is called on application launch.
    virtual void Initialize (CoreApplicationView^ view)
    {
      view->Activated += ref new TypedEventHandler<CoreApplicationView^, IActivatedEventArgs^> (this, &ApplicationView::OnActivated);
    }

    // This method is called after Initialize.
    virtual void SetWindow (CoreWindow^ window)
    {
      main_window = window;
    }

    virtual void Load (Platform::String^ entryPoint)
    {
    }

    virtual void Run()
    {
    }

    // This method is called before the application exits.
    virtual void Uninitialize ()
    {
    }

    /// Main window
    virtual Windows::UI::Core::CoreWindow^ MainWindow ()
    {
      return main_window.Get ();
    }

  private:
    void OnActivated (CoreApplicationView^ applicationView, IActivatedEventArgs^ args)
    {
      CoreWindow::GetForCurrentThread ()->Activate ();
    }

  private:
    Platform::Agile<CoreWindow> main_window;
};

/// Фабрика создания основного окна приложения
ref class ApplicationSource sealed: Windows::ApplicationModel::Core::IFrameworkViewSource
{
  public:
    virtual Windows::ApplicationModel::Core::IFrameworkView^ CreateView ()
    {
      return ref new ApplicationView;
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

namespace syslib
{

namespace android
{

/// Получение контекста приложения
IApplicationContext^ get_context ()
{
  return app_view;
}

}

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
