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
  MSG msg;
  
  bool is_quit = GetMessage (&msg, 0, 0, 0) == 0;

  CheckErrors ("syslib::Win32Platform::DoNextEvent");

  TranslateMessage (&msg);
  DispatchMessage  (&msg);
  SetLastError     (0);
}

void Platform::WaitMessage ()
{
  ::WaitMessage ();
  CheckErrors   ("syslib::Win32Platform::WaitMessage");
}
