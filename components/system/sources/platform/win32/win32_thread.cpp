#include "shared.h"

using namespace syslib;

struct Platform::thread_handle
{
  uintptr_t thread;
};

namespace
{

//функция нити
unsigned CALLBACK thread_run (void* data)
{
  if (!data)
    return 0;

  xtl::com_ptr<IThreadCallback> callback (reinterpret_cast<IThreadCallback*> (data));

  callback->Run ();

  return 0;
}

//получение дескриптора нити
uintptr_t get_handle (Platform::thread_t thread)
{
  return thread ? thread->thread : (uintptr_t)GetCurrentThread ();
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

    handle->thread = _beginthreadex (0, 0, &thread_run, callback.get (), 0, 0);

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
    Отмена нити
*/

void Platform::CancelThread (thread_t thread)
{
  try
  {
    if (!TerminateThread ((HANDLE)get_handle (thread), 0))
      raise_error ("::TerminateThread");
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::Win32Platform::CancelThread");
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
    if (WaitForSingleObject ((HANDLE)get_handle (thread), INFINITE) == WAIT_FAILED)
      raise_error ("::WaitForSingleObject");
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::Win32Platform::JoinThread");
    throw;
  }
}
