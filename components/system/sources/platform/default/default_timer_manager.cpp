#include "shared.h"

using namespace syslib;

/*
    Генерация исключения: работа с многопоточность невозможна для платформы по умолчанию
*/

namespace
{

void raise (const char* method_name)
{
  throw xtl::format_not_supported_exception (method_name, "No timer support for this platform");
}

}

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
