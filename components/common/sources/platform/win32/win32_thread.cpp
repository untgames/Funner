#include "shared.h"

using namespace common;

/*
    Получение идентификатора текущей нити
*/
   
Win32Platform::threadid_t Win32Platform::GetCurrentThreadId ()
{
  return (threadid_t)::GetCurrentThreadId ();
}
