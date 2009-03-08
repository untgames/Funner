#include <ctime>

#include "shared.h"

using namespace syslib;

/*
    Приостановка выполнения приложения на miliseconds милисекунд
*/

void Platform::Sleep (size_t miliseconds)
{
  clock_t end = clock () + CLOCKS_PER_SEC / 1000 * miliseconds;

  while (clock () < end);
}

/*
    Создание / уничтожение таймера
*/

Platform::timer_t Platform::CreateTimer (size_t period_in_milliseconds, TimerHandler handler, void* user_data)
{
  throw xtl::make_not_implemented_exception ("syslib::X11Platform::CreateTimer");
}

void Platform::KillTimer (timer_t handle)
{
  throw xtl::make_not_implemented_exception ("syslib::X11Platform::KillTimer");
}
