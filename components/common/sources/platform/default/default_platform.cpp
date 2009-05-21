#include "shared.h"

using namespace common;

/*
    Получение системного распределителя памяти
*/

ICustomAllocator* DefaultPlatform::GetSystemAllocator ()
{
  return Singleton<MallocAllocator>::InstancePtr ();
}

/*
    Получение файловых систем
*/

ICustomFileSystem* DefaultPlatform::GetFileSystem ()
{
  return Singleton<StdioFileSystem>::InstancePtr ();
}

ICustomFileSystem* DefaultPlatform::GetIOSystem ()
{
  return Singleton<StdioIOSystem>::InstancePtr ();
}

/*
    Работа с блокировками
*/

void DefaultPlatform::InitLockable (lockable_t& lockable)
{
  lockable.data = 0;  
}

void DefaultPlatform::DestroyLockable (lockable_t& lockable)
{
  lockable.data = 0;
}

void DefaultPlatform::Lock (lockable_t& lockable)
{
  static const char* METHOD_NAME = "common::DefaultPlatform::Lock";

  if (lockable.data)
    throw xtl::format_operation_exception (METHOD_NAME, "Recursive lock not supported");

  lockable.data = (void*)~0;
}

void DefaultPlatform::Unlock (lockable_t& lockable)
{
  lockable.data = 0;
}
    
/*
    Получение системного времени
*/
    
size_t DefaultPlatform::GetMilliseconds ()
{
  return size_t (clock () * 1000.0f / CLOCKS_PER_SEC);
}
    