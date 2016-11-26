#include "shared.h"

#include <common/file.h>
#include <common/strlib.h>

using namespace script;
using namespace common;
using namespace xtl;

namespace
{

/*
    Константы (имена библиотек)
*/

const char* COMMON_ACTION_LIBRARY               = "Common.Action";
const char* COMMON_STATIC_ACTION_THREAD_LIBRARY = "Common.ActionThread";

/*
    Регистрация библиотек
*/

}

namespace engine
{

void bind_static_action_thread (Environment& environment)
{
  InvokerRegistry action_thread_lib = environment.CreateLibrary (COMMON_STATIC_ACTION_THREAD_LIBRARY);

  action_thread_lib.Register ("get_Current",    make_const (ActionThread_Current));
  action_thread_lib.Register ("get_Background", make_const (ActionThread_Background));
  action_thread_lib.Register ("get_Main",       make_const (ActionThread_Main));

  environment.RegisterType<ActionThread> (COMMON_STATIC_ACTION_THREAD_LIBRARY);
}

void bind_common_action (Environment& environment)
{
  InvokerRegistry lib = environment.CreateLibrary (COMMON_ACTION_LIBRARY);

  bind_static_action_thread (environment);

  lib.Register ("get_ThreadType",  make_invoker (&Action::ThreadType));
  lib.Register ("get_IsCanceled",  make_invoker (&Action::IsCanceled));
  lib.Register ("get_IsCompleted", make_invoker (&Action::IsCompleted));
  lib.Register ("Wait",            make_invoker (&Action::Wait));
  lib.Register ("Cancel",          make_invoker (&Action::Cancel));

  environment.RegisterType<Action> (COMMON_ACTION_LIBRARY);
}

}
