#include "shared.h"

using namespace syslib;

/*
    Генерация исключения: работа с многопоточность невозможна для платформы по умолчанию
*/

namespace
{

void raise (const char* method_name)
{
  throw xtl::format_not_supported_exception (method_name, "No mutex support for this platform");
}

}

//создание исключающего семафора
mutex_t DefaultThreadManager::CreateMutex ()
{
  raise ("syslib::DefaultThreadManager::CreateMutex");
  return 0;
}

//удаление исключающего семафора
void DefaultThreadManager::DestroyMutex (mutex_t handle)
{
  raise ("syslib::DefaultThreadManager::DestroyMutex");
}

//захват исключающего семафора
void DefaultThreadManager::LockMutex (mutex_t handle)
{
  raise ("syslib::DefaultThreadManager::LockMutex");
}

//захват исключающего семафора с указанием максимального времени ожидания
bool DefaultThreadManager::LockMutex (mutex_t handle, size_t wait_in_milliseconds)
{
  raise ("syslib::DefaultThreadManager::LockMutex");
  return false;
}

//попытка захвата исключающего семафора
bool DefaultThreadManager::TryLockMutex (mutex_t handle)
{
  raise ("syslib::DefaultThreadManager::TryLockMutex");
  return false;
}

//освобождение исключающего семафора
void DefaultThreadManager::UnlockMutex (mutex_t handle)
{
  raise ("syslib::DefaultThreadManager::UnlockMutex");
}
