#include "shared.h"

using namespace syslib;

namespace
{

//функция нити
void* thread_run (void* data)
{
  if (!data)
    return 0;

  xtl::com_ptr<IThreadCallback> callback (reinterpret_cast<IThreadCallback*> (data));

  pthread_setcanceltype (PTHREAD_CANCEL_ASYNCHRONOUS, 0);

  try
  {
    callback->Run ();
  }
  catch (...)
  {
  }

  return 0;
}

//получение дескриптора нити
pthread_t get_handle (Platform::thread_t thread)
{
  return thread ? *(pthread_t*)thread : pthread_self ();
}

}

/*
    Создание / удаление нити
*/

Platform::thread_t Platform::CreateThread (IThreadCallback* in_callback)
{
  try
  {
      //проверка корректности аргументов, захват объекта обслуживания нити

    if (!in_callback)
      throw xtl::make_null_argument_exception ("", "callback");

    xtl::com_ptr<IThreadCallback> callback (in_callback);

      //создание нити

    stl::auto_ptr<pthread_t> handle (new pthread_t);

    int status = pthread_create (handle.get (), 0, &thread_run, callback.get ());

    if (status)
      pthread_raise_error ("::pthread_create", status);
      
    return (thread_t)handle.release ();
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::PThreadPlatform::CreateThread");
    throw;
  }
}

void Platform::DestroyThread (thread_t thread)
{
  if (!thread)
    return;    

  pthread_t* handle = (pthread_t*)thread;    

  delete handle;
}

/*
    Отмена нити
*/

void Platform::CancelThread (thread_t thread)
{
  try
  {
    int status = pthread_cancel (get_handle (thread));

    if (status)
      pthread_raise_error ("::pthread_cancel", status);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::PThreadPlatform::CancelThread");
    throw;
  }    
}

/*
    Ожидание завершения нити
*/

void Platform::JoinThread (thread_t thread)
{
  try
  {
    void* exit_code = 0;

    int status = pthread_join (get_handle (thread), &exit_code);

    if (status)
      pthread_raise_error ("::pthread_join", status);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::PThreadPlatform::JoinThread");
    throw;
  }
}
