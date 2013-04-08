#include "shared.h"

using namespace Windows::UI::Core;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::ApplicationModel::Activation;
using namespace Windows::Foundation;

using namespace syslib;
using namespace syslib::win8;

extern "C" int main (...);

namespace
{

ref class ApplicationView;
ref class ApplicationSource;

ApplicationView^ app_view;
ApplicationSource^ app_source;

/// Слушатель событий приложения
class IApplicationSourceListener
{
  public:
    virtual void OnExit () = 0;
};

/// Основное окно приложения
ref class ApplicationView sealed: Windows::ApplicationModel::Core::IFrameworkView, public IApplicationContext
{
  internal:
    // Constructor
    ApplicationView (ApplicationSource^ in_source) : source (in_source)
    {
      if (app_view)
        throw xtl::format_operation_exception ("syslib::win8::ApplicationView::ApplicationView", "ApplicationView has been already created");

      app_view = this;      
    }

  public:
    // Destructor
    virtual ~ApplicationView ()
    {
      app_view = nullptr;
    }

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

    virtual void Run ()
    {
      char* program_name = "launcher", *env = 0;

      main (0, &program_name, &env);
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
    Platform::Agile<ApplicationSource> source;
    Platform::Agile<CoreWindow>        main_window;
};

/// Фабрика создания основного окна приложения
ref class ApplicationSource sealed: Windows::ApplicationModel::Core::IFrameworkViewSource
{
  internal:
    ApplicationSource () 
      : listener ()
    {
      if (app_source)
        throw xtl::format_operation_exception ("syslib::win8::ApplicationSource::ApplicationSource", "ApplicationSource has been already created");

      app_source = this;

      Windows::ApplicationModel::Core::CoreApplication::Exiting += ref new EventHandler<Platform::Object^> (this, &ApplicationSource::OnExit);
    }

    void SetListener (IApplicationSourceListener* in_listener)
    {
      listener = in_listener;
    }

  public:
    virtual ~ApplicationSource ()
    {
      app_source = nullptr;
    }

    virtual Windows::ApplicationModel::Core::IFrameworkView^ CreateView ()
    {
      return ref new ApplicationView (this);
    }

    void OnExit (Platform::Object^, Platform::Object^)
    {   
      if (listener)
        listener->OnExit ();
    }

  private:
    IApplicationSourceListener* listener;
};

/// Делегат приложения
class Win8ApplicationDelegate: public IApplicationDelegate, public xtl::reference_counter, private IApplicationSourceListener
{
  public:
///Конструктор
    Win8ApplicationDelegate ()
    {
      idle_enabled    = false;
      is_exited       = false;
      listener        = 0;
      wake_up_message = RegisterWindowMessageW (L"Win8ApplicationDelegate.WakeUp");
      main_thread_id  = 0;
      
      if (!wake_up_message)
        raise_error ("::RegisterWindowMessage");

      if (app_source)
        app_source->SetListener (this);
    }

/// Деструктор
    ~Win8ApplicationDelegate ()
    {
      if (app_source)
        app_source->SetListener (0);
    }

/// Главный цикл
    void Run ()
    {
      try
      {
        main_thread_id = GetCurrentThreadId ();

        if (listener)
          listener->OnInitialize ();

        while (!is_exited)
        {
          DoNextEvents ();

           //если нет обработчиков OnIdle - приостанавливаем приложение

          if (!idle_enabled)
          {
            if (!is_exited)
              DoNextEvents ();
          }
          else
          {
            if (listener)
              listener->OnIdle ();
          }
        }
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
      Windows::ApplicationModel::Core::CoreApplication::Exit ();
    }

///Установка необходимости вызова событий idle
    void SetIdleState (bool state)
    {
      idle_enabled = state;
      
      if (state)
      {
        if (!::PostThreadMessage (main_thread_id ? main_thread_id : GetCurrentThreadId (), wake_up_message, 0, 0))
          raise_error ("::PostThreadMessage");
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
/// Оповещение о выходе из цикла
    void OnExit ()
    {
      is_exited = true;
    }

///Обработка следующего события
    void DoNextEvents ()
    {
      try
      {
        if (CoreWindow^ window = CoreWindow::GetForCurrentThread ())
          window->Dispatcher->ProcessEvents (CoreProcessEventsOption::ProcessOneAndAllPending);
      }
      catch (xtl::exception& exception)
      {
        exception.touch ("syslib::Win8ApplicationDelegate::DoNextEvents");
        throw;
      }
    }

  private:
    bool                  idle_enabled;
    bool                  is_exited;
    IApplicationListener* listener;
    UINT                  wake_up_message;
    DWORD                 main_thread_id;
};

}

namespace syslib
{

namespace win8
{

/// Получение контекста приложения
IApplicationContext^ get_context ()
{
  return app_view;
}

/// Старт приложения
void start_application ()
{
  try
  {
    Windows::ApplicationModel::Core::CoreApplication::Run (ref new ApplicationSource);
  }
  catch (Platform::Exception^ e)
  {
    throw xtl::format_operation_exception ("syslib::win8::start_application", "Platform exception %s. %s", tostring (e->Message).c_str (), _com_error (e->HResult).ErrorMessage ());
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::win8::start_application");
    throw;
  }  
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
