#include "shared.h"

using namespace syslib;

struct Platform::thread_handle
{  
  uintptr_t thread;
  size_t    id;
};

namespace
{

//функция нити
unsigned CALLBACK thread_run (void* data)
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
    //подавление всех исключений
  }

  try
  {
    cleanup_tls ();
  }
  catch (...)
  {
    //подавление всех исключений
  }

  return 0;
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

    stl::auto_ptr<thread_handle> handle (new thread_handle);

    handle->thread = _beginthreadex (0, 0, &thread_run, callback.get (), 0, &handle->id);

    if (!handle->thread)
      throw xtl::format_operation_exception ("::_beginthreadex", "%s", common::strerror (errno));

    return handle.release ();
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::Win32Platform::CreateThread");
    throw;
  }
}

void Platform::DestroyThread (thread_t thread)
{
  if (!thread || !thread->thread)
    return;

  CloseHandle ((HANDLE)thread->thread);
  
  delete thread;
}

/*
   Установка приоритета нити
*/

void Platform::SetThreadPriority (thread_t thread, ThreadPriority thread_priority)
{
  try
  {
    if (!thread || !thread->thread)
      throw xtl::make_null_argument_exception ("", "thread");

    int priority;  
            
    switch (thread_priority)
    {
      case ThreadPriority_Low:
        priority = THREAD_PRIORITY_BELOW_NORMAL;
        break;
      case ThreadPriority_Normal:
        priority = THREAD_PRIORITY_NORMAL;
        break;
      case ThreadPriority_High:
        priority = THREAD_PRIORITY_ABOVE_NORMAL;
        break;
      default:
        throw xtl::make_argument_exception ("", "thread_priority", thread_priority);
    }
    
    if (!::SetThreadPriority ((HANDLE)thread->thread, priority))
      raise_error ("::SetThreadPriority");
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::Win32Platform::SetThreadPriority");
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
    if (!thread || !thread->thread)
      throw xtl::make_null_argument_exception ("", "thread");

    if (WaitForSingleObject ((HANDLE)thread->thread, INFINITE) == WAIT_FAILED)
      raise_error ("::WaitForSingleObject");
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::Win32Platform::JoinThread");
    throw;
  }
}

/*
   Получение идентификатора нити
*/

size_t Platform::GetThreadId (thread_t thread)
{
  return (size_t)thread->id;
}

size_t Platform::GetCurrentThreadId ()
{
  return (size_t)::GetCurrentThreadId ();
}
