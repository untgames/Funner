#include "shared.h"

using namespace syslib;

struct Platform::mutex_handle
{
  pthread_mutex_t mutex;
};

//создание исключающего семафора
Platform::mutex_t Platform::CreateMutex ()
{
  try
  {
    thread_init ();

    stl::auto_ptr<mutex_handle> handle (new mutex_handle);

    int status = pthread_mutex_init (&handle->mutex, 0);

    if (status)
      pthread_raise_error ("::pthread_mutex_init", status);

    return handle.release ();
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::PThreadsPlatform::CreateMutex");
    throw;
  }
}

//удаление исключающего семафора
void Platform::DestroyMutex (mutex_t handle)
{
  try
  {
    if (!handle)
      throw xtl::make_null_argument_exception ("", "mutex");

    thread_init ();

    int status = pthread_mutex_destroy (&handle->mutex);

    if (status)
      pthread_raise_error ("::pthread_mutex_destroy", status);

    delete handle;
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::PThreadsPlatform::DestroyMutex");
    throw;
  }
}

//захват исключающего семафора
void Platform::LockMutex (mutex_t handle)
{
  try
  {
    if (!handle)
      throw xtl::make_null_argument_exception ("", "mutex");

    thread_init ();

    int status = pthread_mutex_lock (&handle->mutex);

    if (status)
      pthread_raise_error ("::pthread_mutex_lock", status);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::PThreadsPlatform::LockMutex(mutex_t)");
    throw;
  }
}

//захват исключающего семафора с указанием максимального времени ожидания
void Platform::LockMutex (mutex_t handle, size_t wait_in_milliseconds)
{
  throw xtl::make_not_implemented_exception ("syslib::PThreadsPlatform::LockMutex(mutex_t, size_t)");
}

//попытка захвата исключающего семафора
bool Platform::TryLockMutex (mutex_t handle)
{
  try
  {
    if (!handle)
      throw xtl::make_null_argument_exception ("", "mutex");

    thread_init ();

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
    exception.touch ("syslib::PThreadsPlatform::TryLockMutex");
    throw;
  }
}

//освобождение исключающего семафора
void Platform::UnlockMutex (mutex_t handle)
{
  try
  {
    if (!handle)
      throw xtl::make_null_argument_exception ("", "mutex");

    thread_init ();

    int status = pthread_mutex_unlock (&handle->mutex);

    if (status)
      pthread_raise_error ("::pthread_mutex_unlock", status);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::PThreadsPlatform::UnlockMutex");
    throw;
  }
}
