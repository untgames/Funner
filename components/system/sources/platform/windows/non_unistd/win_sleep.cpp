#include "../shared.h"

using namespace syslib;

/*
    Приостановка выполнения приложения на miliseconds милисекунд
*/

void WindowsApplicationManager::Sleep (size_t miliseconds)
{
  SetLastError (0);
  ::Sleep ((DWORD)miliseconds);
}
