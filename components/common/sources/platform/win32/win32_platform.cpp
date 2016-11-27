#include "shared.h"

using namespace common;

/*
    Получение файловых систем
*/

ICustomFileSystem* Win32Platform::GetFileSystem ()
{
  return &*Singleton<Win32FileSystem>::Instance ();
}
