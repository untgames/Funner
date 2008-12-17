#include "shared.h"

using namespace common;

/*
    Утилиты
*/

namespace
{

#ifdef _WIN32

//функция инициализации библиотеки
inline void thread_init ()
{
  static __declspec(thread) bool is_thread_initialized = false;

  if (is_thread_initialized)
    return;

  pthread_win32_thread_attach_np ();

  is_thread_initialized = true;
}

//функция деинициализации библиотеки
inline void thread_done ()
{
  pthread_win32_thread_detach_np ();
}

inline void process_init ()
{
  pthread_win32_process_attach_np ();
}

inline void process_done ()
{
  pthread_win32_process_detach_np ();
}

#else

inline void thread_init ()
{
}

inline void thread_done ()
{
}

inline void process_init ()
{
}

inline void process_done ()
{
}

#endif

//функция нити
void* thread_run (void* data)
{
  if (!data)
    return 0;

  xtl::com_ptr<IThreadCallback> callback (reinterpret_cast<IThreadCallback*> (data));

  thread_init ();

  callback->Run ();

  thread_done ();

  return 0;
}

}

/*
    Конструктор / деструктор
*/

PThreadsSystem::PThreadsSystem ()
{
  process_init ();
  thread_init ();
}

PThreadsSystem::~PThreadsSystem ()
{
  thread_done ();
  process_done ();
}

/*
    Создание / удаление нити
*/

PThreadsSystem::thread_t PThreadsSystem::CreateThread (IThreadCallback* in_callback, ThreadState initial_state)
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
      throw xtl::format_operation_exception ("", "Can't create thread. Reason: ::pthread_create return %d", status);

    return handle.release ();
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("common::PThreadsSystem::CreateThread");
    throw;
  }
}

void PThreadsSystem::DeleteThread (thread_t thread)
{
  if (!thread)
    return;

  thread_init ();

  pthread_t* handle = (pthread_t*)thread;



  delete handle;
}

/*
    Управление состоянием работы нити
*/

void PThreadsSystem::SetThreadState (thread_t thread, ThreadState state)
{
  thread_init ();
}

ThreadState PThreadsSystem::GetThreadState (thread_t thread)
{
  thread_init ();

  return ThreadState_Exited;
}

/*
    Ожидание завершения нити
*/

void PThreadsSystem::JoinThread (thread_t thread)
{
  static const char* METHOD_NAME = "common::PThreadsSystem::JoinThread";

  if (!thread)
    throw xtl::make_null_argument_exception (METHOD_NAME, "thread");

  thread_init ();

  pthread_t* handle    = (pthread_t*)thread;
  void*      exit_code = 0;

  int status = pthread_join (*handle, &exit_code);

  if (status)
    throw xtl::format_operation_exception (METHOD_NAME, "Can't join threads. Reason: ::pthread_join return %d", status);
}
