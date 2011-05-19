#include "shared.h"

using namespace syslib;

//создание исключающего семафора
mutex_t PThreadManager::CreateMutex ()
{
  try
  {
    stl::auto_ptr<mutex_handle> handle (new mutex_handle);
    
    pthread_mutexattr_t attr;

    int status = pthread_mutexattr_init (&attr);
    
    if (status)
      pthread_raise_error ("::pthread_mutexattr_init", status);           
    
    status = pthread_mutexattr_settype (&attr, PTHREAD_MUTEX_RECURSIVE);
//    status = pthread_mutexattr_settype (&attr, PTHREAD_MUTEX_RECURSIVE_NP);
    
    if (status)
      pthread_raise_error ("::pthread_mutexattr_settype", status);

    status = pthread_mutex_init (&handle->mutex, &attr);

    if (status)
      pthread_raise_error ("::pthread_mutex_init", status);

    return handle.release ();
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::PThreadManager::CreateMutex");
    throw;
  }
}

//удаление исключающего семафора
void PThreadManager::DestroyMutex (mutex_t handle)
{
  try
  {
    if (!handle)
      throw xtl::make_null_argument_exception ("", "mutex");

    int status = pthread_mutex_destroy (&handle->mutex);

    if (status)
      pthread_raise_error ("::pthread_mutex_destroy", status);

    delete handle;
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::PThreadManager::DestroyMutex");
    throw;
  }
}

//захват исключающего семафора
void PThreadManager::LockMutex (mutex_t handle)
{
  try
  {
    if (!handle)
      throw xtl::make_null_argument_exception ("", "mutex");

    int status = pthread_mutex_lock (&handle->mutex);

    if (status)
      pthread_raise_error ("::pthread_mutex_lock", status);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::PThreadManager::LockMutex(mutex_t)");
    throw;
  }
}

//захват исключающего семафора с указанием максимального времени ожидания
bool PThreadManager::LockMutex (mutex_t handle, size_t wait_in_milliseconds)
{
  throw xtl::make_not_implemented_exception ("syslib::PThreadManager::LockMutex(mutex_t, size_t)");
}

//попытка захвата исключающего семафора
bool PThreadManager::TryLockMutex (mutex_t handle)
{
  try
  {
    if (!handle)
      throw xtl::make_null_argument_exception ("", "mutex");

    int status = pthread_mutex_trylock (&handle->mutex);

    switch (status)
    {
      case 0:     return true;
      case EBUSY: return false;
      default:
        pthread_raise_error ("::pthread_mutex_trylock", status);
        return false;
    }
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::PThreadManager::TryLockMutex");
    throw;
  }
}

//освобождение исключающего семафора
void PThreadManager::UnlockMutex (mutex_t handle)
{
  try
  {
    if (!handle)
      throw xtl::make_null_argument_exception ("", "mutex");

    int status = pthread_mutex_unlock (&handle->mutex);

    if (status)
      pthread_raise_error ("::pthread_mutex_unlock", status);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::PThreadManager::UnlockMutex");
    throw;
  }
}
