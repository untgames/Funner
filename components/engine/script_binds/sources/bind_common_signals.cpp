#include "shared.h"

#include <xtl/signal.h>

using namespace script;
using namespace xtl;

namespace
{

/*
    Константы
*/

const char* COMMON_CONNECTION_LIBRARY = "Common.Connection";

}

namespace engine
{

/*
    Регистрация библиотеки работы с деревьями строк
*/

void bind_common_signals (Environment& environment)
{
  InvokerRegistry& lib = environment.CreateLibrary (COMMON_CONNECTION_LIBRARY);

    //регистрация операций
  
  lib.Register ("Disconnect",    make_invoker (&xtl::connection::disconnect));
  lib.Register ("Block",         make_invoker (&xtl::connection::block));
  lib.Register ("Unblock",       make_invoker (&xtl::connection::unblock));
  lib.Register ("get_Blocked",   make_invoker (&xtl::connection::blocked));
  lib.Register ("get_Connected", make_invoker (&xtl::connection::connected));
  lib.Register ("IsEqual",       make_invoker (&xtl::connection::operator ==));

    //регистрация типов данных

  environment.RegisterType<xtl::connection> (COMMON_CONNECTION_LIBRARY);
}

}
