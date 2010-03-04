#include "shared.h"

using namespace syslib;

//создание исключающего семафора
Platform::mutex_t Platform::CreateMutex ()
{
  raise ("syslib::DefaultPlatform::CreateMutex");
  return 0;
}

//удаление исключающего семафора
void Platform::DestroyMutex (mutex_t handle)
{
  raise ("syslib::DefaultPlatform::DestroyMutex");
}

//захват исключающего семафора
void Platform::LockMutex (mutex_t handle)
{
  raise ("syslib::DefaultPlatform::LockMutex");
}

//захват исключающего семафора с указанием максимального времени ожидани¤
void Platform::LockMutex (mutex_t handle, size_t wait_in_milliseconds)
{
  raise ("syslib::DefaultPlatform::LockMutex");
}

//попытка захвата исключающего семафора
bool Platform::TryLockMutex (mutex_t handle)
{
  raise ("syslib::DefaultPlatform::TryLockMutex");
  return false;
}

//освобождение исключающего семафора
void Platform::UnlockMutex (mutex_t handle)
{
  raise ("syslib::DefaultPlatform::UnlockMutex");
}
