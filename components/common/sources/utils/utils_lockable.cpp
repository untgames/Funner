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
  if (!handle)
    return;

  try
  {
    Platform::DestroyLockable (handle);

    handle = 0;
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
  if (!handle)
    return;

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
  if (!handle)
    return;

  try
  {
    Platform::Unlock (handle);
  }
  catch (...)
  {
    //подавление всех исключений
  }
}
