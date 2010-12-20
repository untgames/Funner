#include "shared.h"

using namespace syslib;

/*
    Создание / уничтожение таймера
*/

Platform::timer_t Platform::CreateTimer (size_t, TimerHandler, void*)
{
  throw xtl::make_not_implemented_exception ("syslib::DefaultPlatform::CreateTimer");
}

void Platform::KillTimer (timer_t)
{
  throw xtl::make_not_implemented_exception ("syslib::DefaultPlatform::KillTimer");
}
