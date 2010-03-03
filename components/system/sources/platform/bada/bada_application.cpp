#include "shared.h"

using namespace syslib;

/*
    Работа с очередью сообщений
*/

bool Platform::IsMessageQueueEmpty ()
{
  throw xtl::make_not_implemented_exception ("syslib::BadaPlatform::IsMessageQueueEmpty");
}

void Platform::DoNextEvent ()
{
  throw xtl::make_not_implemented_exception ("syslib::BadaPlatform::DoNextEvent");
}

void Platform::WaitMessage ()
{
  throw xtl::make_not_implemented_exception ("syslib::BadaPlatform::WaitMessage");
}

void Platform::UpdateMessageQueue ()
{
  throw xtl::make_not_implemented_exception ("syslib::BadaPlatform::UpdateMessageQueue");
}

/*
    Запуск приложения
*/

void Platform::RunLoop (IRunLoopContext* context)
{
  throw xtl::make_not_implemented_exception ("syslib::BadaPlatform::RunLoop");
/*  if (!context)
    throw xtl::make_null_argument_exception ("syslib::BadaPlatform::RunLoop", "context");

  context->OnEnterRunLoop ();

  context->DoCustomRunLoop ();*/
}
