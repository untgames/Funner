#include "shared.h"

using namespace syslib;

/*
    Генерация исключения: работа с многопоточность невозможна для платформы по умолчанию
*/

namespace
{

void raise (const char* method_name)
{
  throw xtl::format_not_supported_exception (method_name, "No thread support for this platform");
}

}

/*
    Создание / удаление нити
*/

thread_t DefaultThreadManager::CreateThread (IThreadCallback*)
{
  raise ("syslib::DefaultThreadManager::CreateThread");
  return 0;
}

void DefaultThreadManager::DestroyThread (thread_t)
{
  raise ("syslib::DefaultThreadManager::DestroyThread");
}

/*
    Ожидание завершения нити
*/

void DefaultThreadManager::JoinThread (thread_t)
{
  raise ("syslib::DefaultThreadManager::JoinThread");
}

/*
   Получение идентификатора нити
*/

size_t DefaultThreadManager::GetThreadId (thread_t thread)
{
  raise ("syslib::DefaultThreadManager::GetThreadId");
  return 0;
}

size_t DefaultThreadManager::GetCurrentThreadId ()
{
  return 0;
}
