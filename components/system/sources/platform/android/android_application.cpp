#include "shared.h"

using namespace syslib;

/*
    Создание делегата приложения
*/

IApplicationDelegate* Platform::CreateDefaultApplicationDelegate ()
{
  return create_message_queue_application_delegate ();
}
