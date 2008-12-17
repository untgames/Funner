#include "shared.h"

using namespace common;

/*
    Создание / удаление нити
*/

DefaultThreadSystem::thread_t DefaultThreadSystem::CreateThread (IThreadCallback*, ThreadState)
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

void DefaultThreadSystem::SetThreadState (thread_t, ThreadState)
{
  throw xtl::format_not_supported_exception ("common::DefaultThreadSystem::SetThreadState", "Threads are not supported");
}

ThreadState DefaultThreadSystem::GetThreadState (thread_t)
{
  throw xtl::format_not_supported_exception ("common::DefaultThreadSystem::GetThreadState", "Threads are not supported");
}

/*
    Ожидание завершения нити
*/

void DefaultThreadSystem::JoinThread (thread_t)
{
  throw xtl::format_not_supported_exception ("common::DefaultThreadSystem::JointThread", "Threads are not supported");
}
