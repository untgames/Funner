#ifndef NO_PTHREAD_SEMAPHORES

#include "shared.h"

using namespace syslib;

struct syslib::semaphore_handle
{
  sem_t semaphore;
};

//создание семафора
semaphore_t PThreadManager::CreateSemaphore (size_t initial_value)
{
  try
  {
    stl::auto_ptr<semaphore_handle> handle (new semaphore_handle);

    int status = sem_init (&handle->semaphore, 0, initial_value);

    if (status)
      pthread_raise_error ("::sem_init", status);

    return handle.release ();
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::PThreadManager::CreateSemaphore");
    throw;
  }
}

//уничтожение семафора
void PThreadManager::DestroySemaphore (semaphore_t handle)
{
  try
  {
    int status = sem_destroy (&handle->semaphore);

    if (status)
      pthread_raise_error ("::sem_destroy", status);

    delete handle;
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::PThreadManager::DestroySemaphore");
    throw;
  }
}

//ожидание следующей задачи
void PThreadManager::WaitSemaphore (semaphore_t handle)
{
  try
  {
    int status = sem_wait (&handle->semaphore);

    if (status)
      pthread_raise_error ("::sem_wait", status);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::PThreadManager::WaitSemaphore(semaphore_t)");
    throw;
  }
}

//ожидание следующей задачи с таймаутом
bool PThreadManager::WaitSemaphore (semaphore_t handle, size_t wait_in_milliseconds)
{
  throw xtl::make_not_implemented_exception ("syslib::PThreadManager::WaitSemaphore(semaphore_t, size_t)");
}

//попытка ожидания следующей задачи
bool PThreadManager::TryWaitSemaphore (semaphore_t handle)
{
  try
  {
    int status = sem_trywait (&handle->semaphore);

    switch (status)
    {
      case 0:  return true;
      case -1: return false;
      default:
        pthread_raise_error ("::sem_trywait", status);
        return false;
    }
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::PThreadManager::TryWaitSemaphore");
    throw;
  }
}

//посылка следующей задачи
void PThreadManager::PostSemaphore (semaphore_t handle)
{
  try
  {
    int status = sem_post (&handle->semaphore);

    if (status)
      pthread_raise_error ("::sem_post", status);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::PThreadManager::PostSemaphore");
    throw;
  }
}

#endif
