#include "shared.h"

using namespace common;

/*
    Работа с блокировками
*/

void Win32Platform::InitLockable (lockable_t& lockable)
{
  try
  {
    FAST_MUTEX* mutex = (FAST_MUTEX*)malloc (FAST_MUTEX);

    if (!mutex)
      throw xtl::format_operation_exception ("", "No memory");

    ExInitializeFastMutex (mutex);
      
    lockable.data = mutex;
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("common::Win32Platform::CreateLockable");
    throw;
  }
}

void Win32Platform::DestroyLockable (lockable_t& lockable)
{
  try
  {
    if (!lockable.data)
      throw xtl::make_null_argument_exception ("", "lockable");
      
    if (!CloseHandle ((HANDLE)lockable.data))
      raise_error ("::CloseHandle");
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("common::Win32Platform::DestroyLockable");
    throw;
  }
}

void Win32Platform::Lock (lockable_t& lockable)
{
  try
  {
    if (!lockable.data)
      throw xtl::make_null_argument_exception ("", "lockable");

    switch (WaitForSingleObject ((HANDLE)lockable.data, 0xFFFFFFFF))
    {
      case WAIT_OBJECT_0:
        break;
      default:
        raise_error ("::WaitForSingleObject");
        break;
    }
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("common::Win32Platform::Lock");
    throw;
  }
}

void Win32Platform::Unlock (lockable_t& lockable)
{
  try
  {
    if (!lockable.data)
      throw xtl::make_null_argument_exception ("", "lockable");

    if (!ReleaseMutex ((HANDLE)lockable.data))
      raise_error ("::ReleaseMutex");
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("common::Win32Platform::Unlock");
    throw;
  }
}
