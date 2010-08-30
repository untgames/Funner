#include "shared.h"

using namespace syslib;

struct Platform::condition_handle
{
  pthread_cond_t condition;
};

//создание условия
Platform::condition_t Platform::CreateCondition ()
{
  try
  {
    stl::auto_ptr<condition_handle> handle (new condition_handle);

    int status = pthread_cond_init (&handle->condition, 0);

    if (status)
      pthread_raise_error ("::pthread_cond_init", status);

    return handle.release ();
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::PThreadsPlatform::CreateCondition");
    throw;
  }
}

//удаление условия
void Platform::DestroyCondition (condition_t handle)
{
  try
  {
    if (!handle)
      throw xtl::make_null_argument_exception ("", "condition");

    int status = pthread_cond_destroy (&handle->condition);

    if (status)
      pthread_raise_error ("::pthread_cond_destroy", status);

    delete handle;
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::PThreadsPlatform::DestroyCondition");
    throw;
  }
}

void Platform::WaitCondition (condition_t handle, mutex_t mutex_handle)
{
  try
  {
    if (!handle)
      throw xtl::make_null_argument_exception ("", "condition");

    if (!mutex_handle)
      throw xtl::make_null_argument_exception ("", "mutex");
      
    int status = pthread_cond_wait (&handle->condition, &handle->mutex);

    if (status)
      pthread_raise_error ("::pthread_cond_wait", status);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::PThreadsPlatform::WaitCondition(condition_t)");
    throw;
  }
}

void Platform::WaitCondition (condition_t, mutex_t, size_t wait_in_milliseconds)
{
  throw xtl::make_not_implemented_exception ("syslib::PThreadsPlatform::WaitCondition(condition_t,size_t)");
}

void Platform::NotifyCondition (condition_t handle, bool broadcast)
{
  try
  {
    if (!handle)
      throw xtl::make_null_argument_exception ("", "condition");
    
    if (broadcast)
    {
      int status = pthread_cond_broadcast (&handle->condition);

      if (status)
        pthread_raise_error ("::pthread_cond_broadcast", status);
    }
    else
    {
      int status = pthread_cond_signal (&handle->condition);

      if (status)
        pthread_raise_error ("::pthread_cond_signal", status);
    }
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::PThreadsPlatform::WaitCondition(condition_t,size_t)");
    throw;
  }
}
