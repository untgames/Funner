#include "../shared.h"

using namespace syslib;

struct Platform::condition_handle
{
};

//создание условия
Platform::condition_t Platform::CreateCondition ()
{
  try
  {
    throw xtl::make_not_implemented_exception ("syslib::Win32Platform::CreateCondition");
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::Win32Platform::CreateCondition");
    throw;
  }
}

//удаление условиЯ
void Platform::DestroyCondition (condition_t handle)
{
  try
  {
    throw xtl::make_not_implemented_exception ("syslib::Win32Platform::DestroyCondition");
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::Win32Platform::DestroyCondition");
    throw;
  }
}

void Platform::WaitCondition (condition_t handle, mutex_t mutex_handle)
{
  try
  {
    throw xtl::make_not_implemented_exception ("syslib::Win32Platform::WaitCondition(condition_t,mutex_t)");    
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::Win32Platform::WaitCondition(condition_t)");
    throw;
  }
}

void Platform::WaitCondition (condition_t, mutex_t mutex_handle, size_t wait_in_milliseconds)
{
  throw xtl::make_not_implemented_exception ("syslib::Win32Platform::WaitCondition(condition_t,size_t)");
}

void Platform::NotifyCondition (condition_t handle, bool broadcast)
{
  try
  {
    throw xtl::make_not_implemented_exception ("syslib::Win32Platform::NotifyCondition(condition_t,bool)");
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::Win32Platform::WaitCondition(condition_t,size_t)");
    throw;
  }
}
