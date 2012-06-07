#include "shared.h"

#include <xtl/stat_counter.h>

#include <common/log.h>

using namespace engine;
using namespace script;

namespace common
{

namespace engine_script_bind
{

/*
    Константы
*/

const char* COMPONENT_NAME        = "script.binds.Debug";
const char* BINDER_NAME           = "Debug";
const char* DEBUG_MANAGER_LIBRARY = "Debug";

/*
    Регистрация библиотек
*/

namespace
{

void enum_stat_counters (const xtl::function<void (const char*, const char*, xtl::stat_counter::value_type)>& fn)
{
  for (xtl::stat_counter_iterator i; i; ++i)
    fn (i->group_name (), i->name (), i->value ());
}

}

void bind_debug_manager_library (Environment& environment)
{
  InvokerRegistry lib = environment.Library (DEBUG_MANAGER_LIBRARY);

  lib.Register ("CreateStatCountersEnumerator", make_callback_invoker<void (const char*, const char*, xtl::stat_counter::value_type)> ());
  lib.Register ("EnumStatCounters", make_invoker (enum_stat_counters));
}

/*
    Компонент
*/

class Component
{
  public:
    Component ()
    {
      LibraryManager::RegisterLibrary (BINDER_NAME, &Bind);
    }

  private:
    static void Bind (Environment& environment)
    {
      bind_debug_manager_library (environment);
    }
};

extern "C"
{

common::ComponentRegistrator<Component> DebugScriptBind (COMPONENT_NAME);

}

}

}
