#include "shared.h"

using namespace syslib;

/*
    Работа с очередью сообщений
*/

bool Platform::IsMessageQueueEmpty ()
{
  return true;
}

void Platform::DoNextEvent ()
{
}

void Platform::WaitMessage ()
{
}

void Platform::UpdateMessageQueue ()
{
}
