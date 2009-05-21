#include <xtl/common_exceptions.h>

#include <common/lockable.h>

#include <platform/platform.h>

using namespace common;

/*
    Описание реализации блокируемого объекта
*/

struct Lockable::Impl
{
  Platform::lockable_t lockable; //реализациия блокируемого объекта

///Конструктор
  Impl ()
  {
    Platform::InitLockable (lockable);
  }
  
///Деструктор
  ~Impl ()
  {
    try
    {
      Platform::DestroyLockable (lockable);
    }
    catch (...)
    {
      //подавление всех исключений
    }
  }

///Блокировка
  void Lock ()
  {
    Platform::Lock (lockable);
  }
  
///Отмена блокировки
  void Unlock ()
  {
    try
    {
      Platform::Unlock (lockable);
    }
    catch (...)
    {
      //подавление всех исключений
    }
  }
};

/*
    Конструктор / деструктор
*/

Lockable::Lockable ()
{
  try
  {
    impl = new Impl;
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("common::Lockable::Lockable");
    throw;
  }
}

Lockable::~Lockable ()
{
  delete impl;
}

/*
    Блокировка
*/

void Lockable::Lock ()
{
  try
  {
    impl->Lock ();
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("common::Lockable::Lock");
    throw;
  }
}

void Lockable::Unlock ()
{
  impl->Unlock ();
}
