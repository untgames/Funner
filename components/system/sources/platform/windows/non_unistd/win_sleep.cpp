#include "../shared.h"

using namespace syslib;

/*
    ѕриостановка выполнени€ приложени€ на miliseconds милисекунд
*/

void WindowsApplicationManager::Sleep (size_t miliseconds)
{
  SetLastError (0);
  ::Sleep (miliseconds);
}
