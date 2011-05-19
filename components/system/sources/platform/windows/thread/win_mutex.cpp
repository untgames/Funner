#include "shared.h"

using namespace syslib;

#ifdef CreateMutex
  #undef CreateMutex
#endif

//создание исключающего семафора
mutex_t WindowsThreadManager::CreateMutex ()
{
  try
  {
    stl::auto_ptr<mutex_handle> handle (new mutex_handle);

    handle->mutex = CreateMutexA (0, FALSE, 0);

    if (!handle->mutex)
      raise_error ("::CreateMutex");

    return handle.release ();
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::WindowsThreadManager::CreateMutex");
    throw;
  }
}

//удаление исключающего семафора
void WindowsThreadManager::DestroyMutex (mutex_t handle)
{
  if (!handle || !handle->mutex)
    return;

  CloseHandle (handle->mutex);

  delete handle;
}

//захват исключающего семафора
void WindowsThreadManager::LockMutex (mutex_t handle)
{
  LockMutex (handle, INFINITE);
}

//захват исключающего семафора с указанием максимального времени ожидания
bool WindowsThreadManager::LockMutex (mutex_t handle, size_t wait_in_milliseconds)
{
  try
  {
    if (!handle)
      throw xtl::make_null_argument_exception ("", "mutex");
      
    DWORD status = WaitForSingleObject (handle->mutex, wait_in_milliseconds);
    
    if (status == WAIT_TIMEOUT)
      return false;

    if (status == WAIT_FAILED)
      raise_error ("::WaitForSingleObject");
      
    return true;
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::WindowsThreadManager::LockMutex(mutex_t, size_t)");
    throw;
  }
}

//попытка захвата исключающего семафора
bool WindowsThreadManager::TryLockMutex (mutex_t handle)
{
  try
  {
    if (!handle)
      throw xtl::make_null_argument_exception ("", "mutex");

    DWORD status = WaitForSingleObject (handle->mutex, 0);

    switch (status)
    {
      case WAIT_ABANDONED:
      case WAIT_OBJECT_0:  return true;
      case WAIT_TIMEOUT:   return false;
      default:
        raise_error ("::WaitForSingleObject");
        return false;
    }
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::WindowsThreadManager::TryLockMutex");
    throw;
  }
}

//освобождение исключающего семафора
void WindowsThreadManager::UnlockMutex (mutex_t handle)
{
  try
  {
    if (!handle)
      throw xtl::make_null_argument_exception ("", "mutex");

    if (!ReleaseMutex (handle->mutex))
      raise_error ("::ReleaseMutex");
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::WindowsThreadManager::UnlockMutex");
    throw;
  }
}
