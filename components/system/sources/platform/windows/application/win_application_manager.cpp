#include "../shared.h"

using namespace syslib;

namespace
{

class Win32ApplicationDelegate: public IApplicationDelegate, public xtl::reference_counter
{
  public:
///Конструктор
    Win32ApplicationDelegate ()
    {
      idle_enabled    = false;
      is_exited       = false;
      listener        = 0;
      wake_up_message = RegisterWindowMessageW (L"Win32ApplicationDelegate.WakeUp");
      main_thread_id  = 0;
      
      if (!wake_up_message)
        raise_error ("::RegisterWindowMessage");
    }

///Запуск цикла обработки сообщений
    void Run ()
    {
      try
      {
        main_thread_id = GetCurrentThreadId ();
        
        if (listener)
          listener->OnInitialize ();
        
        while (!is_exited)
        {
          while (!IsMessageQueueEmpty ())
            DoNextEvent ();

           //если нет обработчиков OnIdle - приостанавливаем приложение

          if (!idle_enabled)
          {
            if (IsMessageQueueEmpty () && !is_exited)
              WaitMessage ();
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
        e.touch ("syslib::Win32ApplicationDelegate::Run");
        throw;
      }
    }

///Выход из приложения
    void Exit (int code)
    {
      if (!PostThreadMessage (main_thread_id, WM_QUIT, code, 0))
        raise_error ("::PostThreadMessage");
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
///Проверка очереди событий на пустоту
    bool IsMessageQueueEmpty ()
    {
      if (is_exited)
        return true;
      
      MSG msg;

      bool result = PeekMessage (&msg, 0, 0, 0, PM_NOREMOVE) == 0;

        //сбрасываем все ошибки

      SetLastError (0);

      return result;
    }
    
///Ожидание события
    void WaitMessage ()
    {
#ifndef WINCE
      try
      {
        if (!::WaitMessage ())
          raise_error ("::WaitMessage");
      }
      catch (xtl::exception& exception)
      {
        exception.touch ("syslib::Win32ApplicationDelegate::WaitMessage");
        throw;
      }
#endif
    }
    
///Обработка следующего события
    void DoNextEvent ()
    {
      try
      {
        MSG msg;

        is_exited = GetMessage (&msg, 0, 0, 0) == 0;
        
        if (is_exited && listener)
          listener->OnExit (msg.wParam);

        TranslateMessage (&msg);
        DispatchMessage  (&msg);
        SetLastError     (0);
      }
      catch (xtl::exception& exception)
      {
        exception.touch ("syslib::Win32ApplicationDelegate::DoNextEvent");
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

///Данные приложения
class ApplicationImpl
{
  public:  
///Конструктор
    ApplicationImpl ()
      : screen_saver_state (true)
    {
    }

///Установка состояния скрин-сейвера
    void SetScreenSaverState (bool state)
    {
      screen_saver_state = state;
    }
    
    bool GetScreenSaverState () { return screen_saver_state; }

  private:
    bool screen_saver_state; //состояние скрин-сейвера
};

typedef common::Singleton<ApplicationImpl> ApplicationSingleton;

}

/*
    Создание делегата приложения
*/

IApplicationDelegate* WindowsApplicationManager::CreateDefaultApplicationDelegate ()
{
  return new Win32ApplicationDelegate;
}

/*
   Открытие URL во внешнем браузере
*/

void WindowsApplicationManager::OpenUrl (const char* url)
{
  try
  {
#ifdef WINCE
    char *verb = "open"; 
#else
    wchar_t *verb = L"open"; 
#endif

    SHELLEXECUTEINFO shell_excinfo;
    memset (&shell_excinfo, 0, sizeof (SHELLEXECUTEINFO));
    shell_excinfo.cbSize = sizeof (SHELLEXECUTEINFO);
    shell_excinfo.hwnd   = 0;
    shell_excinfo.lpVerb = (LPCTSTR)verb;
    shell_excinfo.nShow  = SW_SHOWNORMAL;

    ShellExecuteEx (&shell_excinfo);

    int result = (int)ShellExecuteA (0, "open", url, 0, 0, SW_SHOWNORMAL);
//    int result = (int)shell_excinfo.hInstApp;

    if (result > 32)
      return;

    const char* error;

    switch (result)
    {
      case 0:
        error = "The operating system is out of memory or resources.";
        break;
      case ERROR_FILE_NOT_FOUND:
        error = "The specified file was not found.";
        break;
      case ERROR_PATH_NOT_FOUND:
        error = "The specified path was not found.";
        break;
      case ERROR_BAD_FORMAT:
        error = "The .exe file is invalid (non-Win32 .exe or error in .exe image).";
        break;
      case SE_ERR_ACCESSDENIED:
        error = "The operating system denied access to the specified file.";
        break;
      case SE_ERR_ASSOCINCOMPLETE:
        error = "The file name association is incomplete or invalid.";
        break;
      case SE_ERR_DDEBUSY:
        error = "The DDE transaction could not be completed because other DDE transactions were being processed.";
        break;
      case SE_ERR_DDEFAIL:
        error = "The DDE transaction failed.";
        break;
      case SE_ERR_DDETIMEOUT:
        error = "The DDE transaction could not be completed because the request timed out.";
        break;
      case SE_ERR_DLLNOTFOUND:
        error = "The specified DLL was not found.";
        break;
      case SE_ERR_NOASSOC:
        error = "There is no application associated with the given file name extension. This error will also be returned if you attempt to print a file that is not printable.";
        break;
      case SE_ERR_OOM:
        error = "There was not enough memory to complete the operation.";
        break;
      case SE_ERR_SHARE:
        error = "A sharing violation occurred.";
        break;
      default:
        error = "Unknown error";
    }

    throw xtl::format_operation_exception ("::ShellExecute", error);
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::WindowsApplicationManager::OpenUrl");
    throw;
  }
}

/*
    Получение значения переменной среды
*/

#undef GetEnvironmentVariable

stl::string WindowsApplicationManager::GetEnvironmentVariable (const char* name)
{
#ifdef WINCE
  raise_error ("GetEnvironmentVariable dose not supported for this platform\n");
  return "";
#else
  try
  {
    static const int VALUE_BUFFER_SIZE = 8;
    
    stl::string result;
    
    result.fast_resize (VALUE_BUFFER_SIZE);
    
    DWORD result_size = ::GetEnvironmentVariableA (name, &result [0], result.size () + 1);
    
    if (result_size > 0 && result_size <= result.size ())
    {
      result.fast_resize (result_size);
      
      return result;
    }
    
    if (!result_size)
      raise_error ("::GetEnvironmentVariableA");
      
    result.fast_resize (result_size - 1);
    
    result_size = ::GetEnvironmentVariableA (name, &result [0], result.size () + 1);
    
    if (result_size > 0 && result_size == result.size ())
      return result;
      
    throw xtl::format_operation_exception ("", "::GetEnvironmentVariableA failed twice");
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::WindowsApplicationManager::GetEnvironmentVariable");
    throw;
  }
#endif
}

/*
    Управление энергосбережением
*/

void WindowsApplicationManager::SetScreenSaverState (bool state)
{
  try
  {
    ApplicationSingleton::Instance ()->SetScreenSaverState (state);
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::WindowsApplicationManager::SetScreenSaverState");
    throw;
  }
}

bool WindowsApplicationManager::GetScreenSaverState ()
{
  return ApplicationSingleton::Instance ()->GetScreenSaverState ();
}

/*
    Обработка системных сообщений нити
*/

void WindowsApplicationManager::ProcessThreadMessages ()
{
  MSG msg;

  while (::PeekMessageA (&msg, 0, 0, 0, PM_REMOVE))
  {
    TranslateMessage (&msg);
    DispatchMessage  (&msg);
    SetLastError     (0);  
  }
}
