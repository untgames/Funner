#include "shared.h"

using namespace syslib;

/*
    Создание делегата приложения
*/

IApplicationDelegate* Platform::CreateDefaultApplicationDelegate ()
{
  return create_message_queue_application_delegate ();
}

/*
   Открытие URL во внешнем браузере
*/

void Platform::OpenUrl (const char* url)
{
  throw xtl::make_not_implemented_exception ("X11Platform::OpenUrl");
}
