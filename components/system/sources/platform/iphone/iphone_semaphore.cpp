#include "shared.h"

using namespace syslib;

//создание семафора
semaphore_t IPhoneThreadManager::CreateSemaphore (size_t initial_value)
{
  throw xtl::make_not_implemented_exception ("syslib::IPhoneThreadManager::CreateSemaphore");
}

//уничтожение семафора
void IPhoneThreadManager::DestroySemaphore (semaphore_t handle)
{
  throw xtl::make_not_implemented_exception ("syslib::IPhoneThreadManager::DestroySemaphore");
}

//ожидание следующей задачи
void IPhoneThreadManager::WaitSemaphore (semaphore_t handle)
{
  throw xtl::make_not_implemented_exception ("syslib::IPhoneThreadManager::WaitSemaphore (semaphore_t)");
}

//ожидание следующей задачи с таймаутом
bool IPhoneThreadManager::WaitSemaphore (semaphore_t handle, size_t wait_in_milliseconds)
{
  throw xtl::make_not_implemented_exception ("syslib::IPhoneThreadManager::WaitSemaphore (semaphore_t, size_t)");
}

//попытка ожидания следующей задачи
bool IPhoneThreadManager::TryWaitSemaphore (semaphore_t handle)
{
  throw xtl::make_not_implemented_exception ("syslib::IPhoneThreadManager::TryWaitSemaphore");
}

//посылка следующей задачи
void IPhoneThreadManager::PostSemaphore (semaphore_t handle)
{
  throw xtl::make_not_implemented_exception ("syslib::IPhoneThreadManager::PostSemaphore");
}
