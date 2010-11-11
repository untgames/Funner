#include "../shared.h"

using namespace syslib;
using namespace syslib::bada;

struct Platform::mutex_handle
{
  Osp::Base::Runtime::Mutex mutex;
};

//создание исключающего семафора
Platform::mutex_t Platform::CreateMutex ()
{
  try
  {
    stl::auto_ptr<mutex_handle> handle (new mutex_handle);

    result r = handle->mutex.Create ();

    if (IsFailed (r))
      raise ("Osp::Base::Runtime::Mutex::Create", r);

    return handle.release ();
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::BadaPlatform::CreateMutex");
    throw;
  }
}

//удаление исключающего семафора
void Platform::DestroyMutex (mutex_t handle)
{
  if (!handle)
    return;

  delete handle;
}

//захват исключающего семафора
void Platform::LockMutex (mutex_t handle)
{
  LockMutex (handle, INFINITE);
}

//захват исключающего семафора с указанием максимального времени ожидания
bool Platform::LockMutex (mutex_t handle, size_t wait_in_milliseconds)
{
  try
  {
    if (!handle)
      throw xtl::make_null_argument_exception ("", "mutex");

    result r = handle->mutex.Acquire (wait_in_milliseconds);

    if (IsFailed (r))
      raise ("Osp::Base::Runtime::Mutex::Acquire", r);
      
    return true;
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::BadaPlatform::LockMutex(mutex_t, size_t)");
    throw;
  }
}

//попытка захвата исключающего семафора
bool Platform::TryLockMutex (mutex_t handle)
{
  try
  {
    if (!handle)
      throw xtl::make_null_argument_exception ("", "handle");

    result r = handle->mutex.Acquire (0);   //!!!!!!!!need testing

    switch (r)
    {
      case E_SUCCESS: return true;
      case E_TIMEOUT: return false;
      default:
        raise ("Osp::Base::Runtime::Mutex::Acquire", r);
        return false;
    }
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::BadaPlatform::TryLockMutex");
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

    result r = handle->mutex.Release ();

    if (IsFailed (r))
      raise ("Osp::Base::Runtime::Mutex::Release", r);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::BadaPlatform::UnlockMutex");
    throw;
  }
}
