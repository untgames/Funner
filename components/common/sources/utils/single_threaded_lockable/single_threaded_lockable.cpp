#include <xtl/common_exceptions.h>

#include <common/lockable.h>

using namespace common;

namespace
{

/*
    Реализация блокируемого объекта для однопоточной модели
*/

class SingleThreadedLockable: public ILockable
{
  public:
///Конструктор
    SingleThreadedLockable () : locked (false) {}    

///Блокировка
    void Lock ()
    {
      if (locked)
        throw xtl::format_operation_exception ("common::SingleThreadedLockable::Lock", "Recursive lock not supported");
        
      locked = true;
    }
    
///Отмена блокировки
    void Unlock ()
    {
      locked = false;
    }
    
  private:
    bool locked; //флаг блокировки объекта
};

}

namespace common
{

//создание блокируемого объекта
ILockable* create_lockable ()
{
  return new SingleThreadedLockable;
}

}
