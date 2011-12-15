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

  thread_init ();

  pthread_cleanup_push  (&thread_done, 0);
  

  try
  {
    callback->Run ();
  }
  catch (...)
  {
    thread_done (0);    

  }

  pthread_cleanup_pop (1);

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

      //инициализации библиотеки

    thread_init ();

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

    thread_init ();

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

/*
   Установка приоритета нити
*/

void PThreadManager::SetThreadPriority (thread_t thread, ThreadPriority thread_priority)
{
  try
  {
    if (!thread || !thread->thread)
      throw xtl::make_null_argument_exception ("", "thread");

    if (!thread->default_scheduling_getted)
    {
      sched_param default_scheduling_param;

      int status = pthread_getschedparam (thread->thread, &thread->scheduling_policy, &default_scheduling_param);

      if (status)
        pthread_raise_error ("::pthread_getschedparam", status);

      thread->normal_priority = default_scheduling_param.sched_priority;

      thread->default_scheduling_getted = true;
    }

#ifdef __APPLE__
    int scheduling_policy = SCHED_RR;
#else
    int scheduling_policy = thread->scheduling_policy;
#endif

    sched_param scheduling_param;

    switch (thread_priority)
    {
      case ThreadPriority_Low:
      {
        int min_priority = sched_get_priority_min (scheduling_policy);

        if (min_priority == -1)
          pthread_raise_error ("::sched_get_priority_min", errno);

        scheduling_param.sched_priority = min_priority;
        break;
      }
      case ThreadPriority_Normal:
        scheduling_param.sched_priority = thread->normal_priority;
        break;
      case ThreadPriority_High:
      {
        int max_priority = sched_get_priority_max (scheduling_policy);

        if (max_priority == -1)
          pthread_raise_error ("::sched_get_priority_max", errno);

        scheduling_param.sched_priority = max_priority;
        break;
      }
      default:
        throw xtl::make_argument_exception ("", "thread_priority", thread_priority);
    }

    int status = pthread_setschedparam ((pthread_t)thread->thread, scheduling_policy, &scheduling_param);

    if (status)
      pthread_raise_error ("::pthread_setshedparam", status);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::PThreadManager::SetThreadPriority");
    throw;
  }
}
