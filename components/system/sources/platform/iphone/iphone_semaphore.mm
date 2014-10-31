#include "shared.h"

using namespace syslib;

struct syslib::semaphore_handle
{
  dispatch_semaphore_t semaphore;
};

//создание семафора
semaphore_t IPhoneThreadManager::CreateSemaphore (size_t initial_value)
{
  stl::auto_ptr<semaphore_handle> handle (new semaphore_handle);

  handle->semaphore = dispatch_semaphore_create (initial_value);

  if (!handle->semaphore)
    throw xtl::format_operation_exception ("syslib::IPhoneThreadManager::CreateSemaphore", "Can't create semaphore, ::dispatch_semaphore_create failed");

  return handle.release ();
}

//уничтожение семафора
void IPhoneThreadManager::DestroySemaphore (semaphore_t handle)
{
  if (!handle || !handle->semaphore)
    return;

  dispatch_release (handle->semaphore);

  delete handle;
}

//ожидание следующей задачи
void IPhoneThreadManager::WaitSemaphore (semaphore_t handle)
{
  if (!handle)
    throw xtl::make_null_argument_exception ("syslib::IPhoneThreadManager::WaitSemaphore (semaphore_t)", "handle");

  dispatch_semaphore_wait (handle->semaphore, DISPATCH_TIME_FOREVER);
}

//ожидание следующей задачи с таймаутом
bool IPhoneThreadManager::WaitSemaphore (semaphore_t handle, size_t wait_in_milliseconds)
{
  if (!handle)
    throw xtl::make_null_argument_exception ("syslib::IPhoneThreadManager::WaitSemaphore (semaphore_t, size_t))", "handle");

  return dispatch_semaphore_wait (handle->semaphore, dispatch_time (DISPATCH_TIME_NOW, wait_in_milliseconds * 1000000)) == 0;
}

//попытка ожидания следующей задачи
bool IPhoneThreadManager::TryWaitSemaphore (semaphore_t handle)
{
  if (!handle)
    throw xtl::make_null_argument_exception ("syslib::IPhoneThreadManager::TryWaitSemaphore", "handle");

  return dispatch_semaphore_wait (handle->semaphore, DISPATCH_TIME_NOW) == 0;
}

//посылка следующей задачи
void IPhoneThreadManager::PostSemaphore (semaphore_t handle)
{
  if (!handle)
    throw xtl::make_null_argument_exception ("syslib::IPhoneThreadManager::PostSemaphore", "handle");

  dispatch_semaphore_signal (handle->semaphore);
}
