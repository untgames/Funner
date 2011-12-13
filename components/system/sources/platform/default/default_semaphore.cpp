#include "shared.h"

using namespace syslib;

/*
    Генерация исключения: работа с многопоточность невозможна для платформы по умолчанию
*/

namespace
{

void raise (const char* method_name)
{
  throw xtl::format_not_supported_exception (method_name, "No semaphore support for this platform");
}

}

//создание семафора
semaphore_t DefaultThreadManager::CreateSemaphore (size_t initial_value)
{
  raise ("syslib::DefaultThreadManager::CreateSemaphore");
  return 0;
}

//уничтожение семафора
void DefaultThreadManager::DestroySemaphore (semaphore_t handle)
{
  raise ("syslib::DefaultThreadManager::DestroySemaphore");
}

//ожидание следующей задачи
void DefaultThreadManager::WaitSemaphore (semaphore_t handle)
{
  raise ("syslib::DefaultThreadManager::WaitSemaphore");
}

//ожидание следующей задачи с таймаутом
bool DefaultThreadManager::WaitSemaphore (semaphore_t handle, size_t wait_in_milliseconds)
{
  raise ("syslib::DefaultThreadManager::WaitSemaphore");
  return false;
}

//попытка ожидани¤ следующей задачи
bool DefaultThreadManager::TryWaitSemaphore (semaphore_t handle)
{
  raise ("syslib::DefaultThreadManager::TryWaitSemaphore");
  return false;
}

//посылка следующей задачи
void DefaultThreadManager::PostSemaphore (semaphore_t handle)
{
  raise ("syslib::DefaultThreadManager::PostSemaphore");
}
