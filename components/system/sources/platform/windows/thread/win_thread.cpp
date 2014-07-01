#include "shared.h"

using namespace syslib;

struct syslib::thread_handle
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

thread_t WindowsThreadManager::CreateThread (IThreadCallback* in_callback)
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
    exception.touch ("syslib::WindowsThreadManager::CreateThread");
    throw;
  }
}

void WindowsThreadManager::DestroyThread (thread_t thread)
{
  if (!thread || !thread->thread)
    return;

  CloseHandle ((HANDLE)thread->thread);

  delete thread;
}

/*
    Ожидание завершения нити
*/

void WindowsThreadManager::JoinThread (thread_t thread)
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
    exception.touch ("syslib::WindowsThreadManager::JoinThread");
    throw;
  }
}

/*
   Получение идентификатора нити
*/

size_t WindowsThreadManager::GetThreadId (thread_t thread)
{
  return (size_t)thread->id;
}

size_t WindowsThreadManager::GetCurrentThreadId ()
{
  return (size_t)::GetCurrentThreadId ();
}

/*
   Установка приоритета нити
*/

void WindowsThreadManager::SetThreadPriority (thread_t thread, ThreadPriority thread_priority)
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
    exception.touch ("syslib::WindowsThreadManager::SetThreadPriority");
    throw;
  }
}

void WindowsThreadManager::SetThreadAffinity (thread_t thread, size_t affinity)
{
  try
  {
    if (!thread || !thread->thread)
      throw xtl::make_null_argument_exception ("", "thread");

    if (!::SetThreadAffinityMask ((HANDLE)thread->thread, (DWORD_PTR)affinity))
      raise_error ("::SetThreadAffinityMask");
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::WindowsThreadManager::SetThreadAffinity");
    throw;
  }
}
