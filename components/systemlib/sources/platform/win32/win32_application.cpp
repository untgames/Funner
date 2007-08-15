#include "shared.h"

using namespace syslib;

/*
    ѕриостановка выполнени€ приложени€ на miliseconds милисекунд
*/

void Win32Platform::Sleep (size_t miliseconds)
{
  ::Sleep (miliseconds);
  CheckErrors ("syslib::Win32Platform::Sleep");
}

/*
    –абота с очередью сообщений
*/

bool Win32Platform::IsMessageQueueEmpty ()
{
  MSG msg;
  
  bool result = PeekMessage (&msg, 0, 0, 0, PM_NOREMOVE) == 0;

  CheckErrors ("syslib::Win32Platform::IsMessageQueueEmpty");
  
  return result;
}

void Win32Platform::DoNextEvent ()
{
  MSG msg;
  
  bool is_quit = GetMessage (&msg, 0, 0, 0) == 0;

  CheckErrors ("syslib::Win32Platform::DoNextEvent");  

  TranslateMessage (&msg);

  if (DispatchMessage  (&msg)) CheckErrors ("syslib::Win32Platform::DoNextEvent");
  else                         SetLastError (0);
}

void Win32Platform::WaitMessage ()
{
  ::WaitMessage ();
  CheckErrors   ("syslib::Win32Platform::WaitMessage");
}
