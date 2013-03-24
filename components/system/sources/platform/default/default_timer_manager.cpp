#include "shared.h"

namespace syslib
{

/*
    Создание / уничтожение таймера
*/

syslib::timer_t DefaultTimerManager::CreateTimer (size_t, TimerHandler, void*)
{
  throw xtl::make_not_implemented_exception ("syslib::DefaultTimerManager::CreateTimer");
}

void DefaultTimerManager::KillTimer (timer_t)
{
  throw xtl::make_not_implemented_exception ("syslib::DefaultTimerManager::KillTimer");
}

}
