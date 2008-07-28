#include "shared.h"

using namespace syslib;

/*
    Работа с очередью сообщений
*/

bool Platform::IsMessageQueueEmpty ()
{
  MSG msg;
  
  bool result = PeekMessage (&msg, 0, 0, 0, PM_NOREMOVE) == 0;
  
    //сбрасываем все ошибки
  
  SetLastError (0);
  
  return result;
}

void Platform::DoNextEvent ()
{
  try
  {
    MSG msg;
    
    bool is_quit = GetMessage (&msg, 0, 0, 0) == 0;

    if (is_quit)
      syslib::Application::Exit (msg.wParam);

    check_errors ("::GetMessage");

    TranslateMessage (&msg);
    DispatchMessage  (&msg);
    SetLastError     (0);        
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::Win32Platform::DoNextEvent");
    throw;
  }
}

void Platform::WaitMessage ()
{
  try
  {
    if (!::WaitMessage ())
      raise_error ("::WaitMessage");
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::Win32Platform::WaitMessage");
    throw;
  }
}
