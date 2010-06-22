#include "shared.h"

using namespace syslib;

//создание семафора
Platform::semaphore_t Platform::CreateSemaphore (size_t initial_value)
{
  raise ("syslib::DefaultPlatform::CreateSemaphore");
  return 0;
}

//уничтожение семафора
void Platform::DestroySemaphore (semaphore_t handle)
{
  raise ("syslib::DefaultPlatform::DestroySemaphore");
}

//ожидание следующей задачи
void Platform::WaitSemaphore (semaphore_t handle)
{
  raise ("syslib::DefaultPlatform::WaitSemaphore");
}

//ожидание следующей задачи с таймаутом
void Platform::WaitSemaphore (semaphore_t handle, size_t wait_in_milliseconds)
{
  raise ("syslib::DefaultPlatform::WaitSemaphore");
}

//попытка ожидани¤ следующей задачи
bool Platform::TryWaitSemaphore (semaphore_t handle)
{
  raise ("syslib::DefaultPlatform::TryWaitSemaphore");
  return false;
}

//посылка следующей задачи
void Platform::PostSemaphore (semaphore_t handle)
{
  raise ("syslib::DefaultPlatform::PostSemaphore");
}
