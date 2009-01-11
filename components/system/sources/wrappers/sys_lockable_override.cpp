#include "shared.h"

using namespace syslib;
using namespace common;

namespace
{

/*
    Реализация блокируемого объекта для многопоточной модели
*/

class MultiThreadedLockable: public ILockable
{
  public:
///Блокировка
    void Lock ()
    {
      mutex.Lock ();
    }
    
///Отмена блокировки
    void Unlock ()
    {
      mutex.Unlock ();
    }
    
  private:
    Mutex mutex;
};

}

namespace common
{

//создание блокируемого объекта
ILockable* create_lockable ()
{
  return new MultiThreadedLockable;
}

}
