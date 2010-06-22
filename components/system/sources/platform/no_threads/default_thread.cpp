#include "shared.h"

using namespace syslib;

/*
    Создание / удаление нити
*/

Platform::thread_t Platform::CreateThread (IThreadCallback*)
{
  raise ("syslib::DefaultPlatform::CreateThread");
  return 0;
}

void Platform::DestroyThread (thread_t)
{
  raise ("syslib::DefaultPlatform::DestroyThread");
}

/*
    Ожидание завершения нити
*/

void Platform::JoinThread (thread_t)
{
  raise ("syslib::DefaultPlatform::JoinThread");
}

/*
   Получение идентификатора нити
*/

size_t Platform::GetThreadId (thread_t thread)
{
  raise ("syslib::DefaultPlatform::GetThreadId");
  return 0;
}

size_t Platform::GetCurrentThreadId ()
{
  return 0;
}
