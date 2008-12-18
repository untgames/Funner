#include "shared.h"

using namespace common;

/*
    Создание / удаление нити
*/

DefaultThreadSystem::thread_t DefaultThreadSystem::CreateThread (IThreadCallback*)
{
  throw xtl::format_not_supported_exception ("common::DefaultThreadSystem::CreateThread", "Threads are not supported");
}

void DefaultThreadSystem::DeleteThread (thread_t)
{
  throw xtl::format_not_supported_exception ("common::DefaultThreadSystem::DeleteThread", "Threads are not supported");
}

/*
    Управление состоянием работы нити
*/

void DefaultThreadSystem::CancelThread (thread_t)
{
  throw xtl::format_not_supported_exception ("common::DefaultThreadSystem::CancelThread", "Threads are not supported");
}

/*
    Ожидание завершения нити
*/

void DefaultThreadSystem::JoinThread (thread_t)
{
  throw xtl::format_not_supported_exception ("common::DefaultThreadSystem::JointThread", "Threads are not supported");
}

/*
    Получение текущей нити
*/

ICustomThreadSystem::thread_t DefaultThreadSystem::GetCurrentThread ()
{
  throw xtl::format_not_supported_exception ("common::DefaultThreadSystem::GetCurrentThread", "Threads are not supported");
}

/*
    Работа с критическими секциями кода
*/

void DefaultThreadSystem::EnterCriticalSection ()
{
  throw xtl::format_not_supported_exception ("common::DefaultThreadSystem::EnterCriticalSection", "Threads are not supported");  
}

void DefaultThreadSystem::ExitCriticalSection ()
{
  throw xtl::format_not_supported_exception ("common::DefaultThreadSystem::ExitCriticalSection", "Threads are not supported");  
}
