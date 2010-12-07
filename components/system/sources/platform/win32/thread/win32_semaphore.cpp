#include "shared.h"

using namespace syslib;

struct Platform::semaphore_handle
{
  HANDLE semaphore;
};

#ifdef CreateSemaphore
  #undef CreateSemaphore
#endif

//создание семафора
Platform::semaphore_t Platform::CreateSemaphore (size_t initial_value)
{
  try
  {
    stl::auto_ptr<semaphore_handle> handle (new semaphore_handle);

    handle->semaphore = ::CreateSemaphoreA (0, initial_value, LONG_MAX, 0);

    if (!handle->semaphore)
      raise_error ("::CreateSemaphore");

    return handle.release ();
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::Win32Platform::CreateSemaphore");
    throw;
  }
}

//уничтожение семафора
void Platform::DestroySemaphore (semaphore_t handle)
{
  if (!handle || !handle->semaphore)
    return;

  CloseHandle (handle->semaphore);

  delete handle;
}

//ожидание следующей задачи
void Platform::WaitSemaphore (semaphore_t handle)
{
  WaitSemaphore (handle, INFINITE);
}

//ожидание следующей задачи с таймаутом
bool Platform::WaitSemaphore (semaphore_t handle, size_t wait_in_milliseconds)
{
  try
  {
    if (!handle)
      throw xtl::make_null_argument_exception ("", "handle");
      
    int status = WaitForSingleObject (handle->semaphore, wait_in_milliseconds);
    
    if (status == WAIT_TIMEOUT)
      return false;

    if (status == WAIT_FAILED)
      raise_error ("::WaitForSingleObject");
      
    return true;
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::Win32Platform::WaitSemaphore (semaphore_t, size_t)");
    throw;
  }
}

//попытка ожидания следующей задачи
bool Platform::TryWaitSemaphore (semaphore_t handle)
{
  try
  {
    if (!handle)
      throw xtl::make_null_argument_exception ("", "handle");

    DWORD status = WaitForSingleObject (handle->semaphore, 0);

    switch (status)
    {
      case WAIT_OBJECT_0:  return true;
      case WAIT_TIMEOUT:   return false;
      default:
        raise_error ("::WaitForSingleObject");
        return false;
    }
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::Win32Platform::TryLockSemaphore");
    throw;
  }
}

//посылка следующей задачи
void Platform::PostSemaphore (semaphore_t handle)
{
  try
  {
    if (!handle)
      throw xtl::make_null_argument_exception ("", "handle");

    if (!ReleaseSemaphore (handle->semaphore, 1, 0))
      raise_error ("::ReleaseSemaphore");
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::Win32Platform::PostSemaphore");
    throw;
  }
}
