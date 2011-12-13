#include "shared.h"

using namespace syslib;

/*
    Генерация исключения: работа с многопоточность невозможна для платформы по умолчанию
*/

namespace
{

void raise (const char* method_name)
{
  throw xtl::format_not_supported_exception (method_name, "No thread condition support for this platform");
}

}

//создание условия
condition_t DefaultThreadManager::CreateCondition ()
{
  raise ("syslib::DefaultThreadManager::CreateCondition");
  return 0;
}

//удаление условия
void DefaultThreadManager::DestroyCondition (condition_t handle)
{
  raise ("syslib::DefaultThreadManager::DestroyCondition");
}

void DefaultThreadManager::WaitCondition (condition_t handle, mutex_t mutex_handle)
{
  raise ("syslib::DefaultThreadManager::WaitCondition");
}

bool DefaultThreadManager::WaitCondition (condition_t handle, mutex_t mutex_handle, size_t wait_in_milliseconds)
{
  raise ("syslib::DefaultThreadManager::CreateCondition");
  return false;
}

void DefaultThreadManager::NotifyCondition (condition_t handle, bool broadcast)
{
  raise ("syslib::DefaultThreadManager::NotifyCondition");
}
