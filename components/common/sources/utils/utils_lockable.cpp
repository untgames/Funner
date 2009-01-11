#include <stl/auto_ptr.h>

#include <xtl/common_exceptions.h>
#include <xtl/reference_counter.h>

#include <common/lockable.h>

using namespace common;

/*
    Описание реализации блокируемого объекта
*/

typedef stl::auto_ptr<ILockable> LockablePtr;

struct Lockable::Impl
{
  LockablePtr lockable; //реализациия блокируемого объекта

///Конструктор
  Impl ()
  {
    lockable = create_lockable ();

    if (!lockable)
      throw xtl::format_operation_exception ("", "Lockable not created");
  }

///Блокировка
  void Lock ()
  {
    lockable->Lock ();
  }
  
///Отмена блокировки
  void Unlock ()
  {
    try
    {
      lockable->Unlock ();
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
