#include "shared.h"

using namespace common;

/*
    Получение системного распределителя памяти
*/

ICustomAllocator* DefaultPlatform::GetSystemAllocator ()
{
  return &*Singleton<MallocAllocator>::Instance ();
}

/*
    Получение файловых систем
*/

ICustomFileSystem* DefaultPlatform::GetFileSystem ()
{
  return &*Singleton<StdioFileSystem>::Instance ();
}

ICustomFileSystem* DefaultPlatform::GetIOSystem ()
{
  return &*Singleton<StdioIOSystem>::Instance ();
}

/*
    Работа с блокировками
*/

void DefaultPlatform::InitLockable (lockable_t& lockable)
{
  lockable = 0;
}

void DefaultPlatform::DestroyLockable (lockable_t& lockable)
{
  lockable = 0;
}

void DefaultPlatform::Lock (lockable_t& lockable)
{
  static const char* METHOD_NAME = "common::DefaultPlatform::Lock";

  if (lockable)
    throw xtl::format_operation_exception (METHOD_NAME, "Recursive lock not supported");

  lockable = (void*)~0;
}

void DefaultPlatform::Unlock (lockable_t& lockable)
{
  lockable = 0;
}
    
/*
    Получение системного времени
*/
    
size_t DefaultPlatform::GetMilliseconds ()
{
  return size_t (clock () * 1000.0 / CLOCKS_PER_SEC);
}

/*
    Получение идентификатора текущей нити
*/
   
DefaultPlatform::threadid_t DefaultPlatform::GetCurrentThreadId ()
{
  return 0;
}
