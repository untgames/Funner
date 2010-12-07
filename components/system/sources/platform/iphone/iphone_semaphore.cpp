#include "shared.h"

using namespace syslib;

//создание семафора
Platform::semaphore_t Platform::CreateSemaphore (size_t initial_value)
{
  throw xtl::make_not_implemented_exception ("syslib::iPhonePlatform::CreateSemaphore");
}

//уничтожение семафора
void Platform::DestroySemaphore (semaphore_t handle)
{
  throw xtl::make_not_implemented_exception ("syslib::iPhonePlatform::DestroySemaphore");
}

//ожидание следующей задачи
void Platform::WaitSemaphore (semaphore_t handle)
{
  throw xtl::make_not_implemented_exception ("syslib::iPhonePlatform::WaitSemaphore (semaphore_t)");
}

//ожидание следующей задачи с таймаутом
bool Platform::WaitSemaphore (semaphore_t handle, size_t wait_in_milliseconds)
{
  throw xtl::make_not_implemented_exception ("syslib::iPhonePlatform::WaitSemaphore (semaphore_t, size_t)");
}

//попытка ожидания следующей задачи
bool Platform::TryWaitSemaphore (semaphore_t handle)
{
  throw xtl::make_not_implemented_exception ("syslib::iPhonePlatform::TryWaitSemaphore");
}

//посылка следующей задачи
void Platform::PostSemaphore (semaphore_t handle)
{
  throw xtl::make_not_implemented_exception ("syslib::iPhonePlatform::PostSemaphore");
}
