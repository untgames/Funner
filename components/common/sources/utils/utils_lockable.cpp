#include <xtl/common_exceptions.h>

#include <common/lockable.h>

#include <platform/platform.h>

using namespace common;

/*
    Конструктор / деструктор
*/

Lockable::Lockable ()
{
  try
  {
    Platform::InitLockable (handle);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("common::Lockable::Lockable");
    throw;
  }
}

Lockable::~Lockable ()
{
  try
  {
    Platform::DestroyLockable (handle);
  }
  catch (...)
  {
    //подавление всех исключений
  }
}

/*
    Блокировка
*/

void Lockable::Lock ()
{
  try
  {
    Platform::Lock (handle);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("common::Lockable::Lock");
    throw;
  }
}

void Lockable::Unlock ()
{
  try
  {
    Platform::Unlock (handle);
  }
  catch (...)
  {
    //подавление всех исключений
  }
}
