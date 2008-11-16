#include "shared.h"

#include <syslib/application.h>

using namespace script;
using namespace syslib;

namespace
{

/*
    Константы
*/

const char* APPLICATION_LIBRARY       = "System.Application";
const char* APPLICATION_EVENT_LIBRARY = "System.ApplicationEvent";
const char* COMPONENT_NAME            = "script.binds.System";
const char* BINDER_NAME               = "System";

/*
    Утилиты
*/

void bind_application_events_library (Environment& environment)
{
  InvokerRegistry& lib = environment.Library (APPLICATION_EVENT_LIBRARY);

  lib.Register ("get_OnExit", make_const (ApplicationEvent_OnExit));
  lib.Register ("get_OnIdle", make_const (ApplicationEvent_OnIdle));
}

void bind_application_library (Environment& environment)
{
  InvokerRegistry& lib = environment.Library (APPLICATION_LIBRARY);

    //регистрация операций

  lib.Register ("Exit",                 make_invoker (&syslib::Application::Exit));
  lib.Register ("Sleep",                make_invoker (&syslib::Application::Sleep));
  lib.Register ("CreateEventHandler",   make_callback_invoker<syslib::Application::EventHandler::signature_type> ());
  lib.Register ("RegisterEventHandler", make_invoker (&syslib::Application::RegisterEventHandler));
}

void bind_syslib_library (Environment& environment)
{
  bind_application_events_library (environment);
  bind_application_library        (environment);
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
      bind_syslib_library (environment);
    }
};

extern "C"
{

common::ComponentRegistrator<Component> SystemScriptBind (COMPONENT_NAME);

}

}
