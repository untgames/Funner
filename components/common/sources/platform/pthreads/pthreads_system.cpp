#include "shared.h"

using namespace common;

/*
    Утилиты
*/

namespace
{

//генерация исключения с кодом ошибки
void raise (const char* source, int status)
{
  throw xtl::format_operation_exception (source, "Operation failed. Reason: %s (function exit with code %d)", common::strerror (status), status);
}

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
inline void thread_done (void*)
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

inline void thread_done (void*)
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

  pthread_cleanup_push (&thread_done, 0);

  pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, 0);   //???переделать!!!

  try
  {
    callback->Run ();
  }
  catch (...)
  {
    thread_done (0);
    throw;
  }

  pthread_cleanup_pop (1);

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
  thread_done (0);
  process_done ();
}

/*
    Создание / удаление нити
*/

PThreadsSystem::thread_t PThreadsSystem::CreateThread (IThreadCallback* in_callback)
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
      raise ("::pthread_create", status);
      


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

  pthread_t* handle = (pthread_t*)thread;    

  delete handle;
}

/*
    Отмена нити
*/

void PThreadsSystem::CancelThread (thread_t thread)
{
  try
  {
    if (!thread)
      throw xtl::make_null_argument_exception ("", "thread");
      
    thread_init ();
    
    pthread_t* handle = (pthread_t*)thread;
    
    int status = pthread_cancel (*handle);    

    if (status)
      raise ("::pthread_cancel", status);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("common::PThreadsSystem::CancelThread");
    throw;
  }    
}

/*
    Ожидание завершения нити
*/

void PThreadsSystem::JoinThread (thread_t thread)
{
  try
  {
    if (!thread)
      throw xtl::make_null_argument_exception ("", "thread");
      
    thread_init ();
    
    pthread_t* handle    = (pthread_t*)thread;
    void*      exit_code = 0;

    int status = pthread_join (*handle, &exit_code);

    if (status)
      raise ("::pthread_join", status);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("common::PThreadsSystem::JoinThread");
    throw;
  }
}
