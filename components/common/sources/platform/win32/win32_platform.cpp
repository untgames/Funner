#include "shared.h"

using namespace common;

/*
    Получение файловых систем
*/

ICustomFileSystem* Win32Platform::GetFileSystem ()
{
#ifdef WINCE
  return &*Singleton<Win32FileSystem>::Instance ();
#else
  return DefaultPlatform::GetFileSystem ();
#endif
}
