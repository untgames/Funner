#include "shared.h"

using namespace common;

/*
    Работа с блокировками
*/

void Win32Platform::InitLockable (lockable_t& lockable)
{
  try
  {
    lockable.data = CreateMutex (0, FALSE, 0);

    if (!lockable.data)
      raise_error ("::CreateMutex");
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
