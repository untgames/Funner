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

  try
  {
    callback->Run ();
  }
  catch (...)
  {
  }

  return 0;
}

}

/*
    Создание / удаление нити
*/

thread_t PThreadManager::CreateThread (IThreadCallback* in_callback)
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
    exception.touch ("syslib::PThreadManager::CreateThread");
    throw;
  }
}

void PThreadManager::DestroyThread (thread_t thread)
{
  if (!thread)
    return;    

  pthread_t* handle = (pthread_t*)thread;    

  delete handle;
}

/*
    Ожидание завершения нити
*/

void PThreadManager::JoinThread (thread_t thread)
{
  try
  {
    if (!thread)
      throw xtl::make_null_argument_exception ("", "thread");

    void* exit_code = 0;

    int status = pthread_join (*(pthread_t*)thread, &exit_code);

    if (status)
      pthread_raise_error ("::pthread_join", status);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::PThreadManager::JoinThread");
    throw;
  }
}

/*
   Получение идентификатора нити
*/

size_t PThreadManager::GetThreadId (thread_t thread)
{
  return (size_t)*(pthread_t*)thread;
}

size_t PThreadManager::GetCurrentThreadId ()
{
  return (size_t)pthread_self ();
}
