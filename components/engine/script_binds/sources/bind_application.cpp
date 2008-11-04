#include "shared.h"

#include <syslib/application.h>

using namespace script;
using namespace syslib;

namespace
{

/*
    Константы
*/

const char* APPLICATION_LIBRARY = "System.Application";
const char* COMPONENT_NAME      = "script.binds.System";
const char* BINDER_NAME         = "System";

/*
    Утилиты
*/

void bind_application_library (Environment& environment)
{
  InvokerRegistry& application_lib = environment.Library (APPLICATION_LIBRARY);
  
    //регистрация операций
    
  application_lib.Register ("Exit",  make_invoker (&syslib::Application::Exit));
  application_lib.Register ("Sleep", make_invoker (&syslib::Application::Sleep));
}

void bind_syslib_library (Environment& environment)
{
  bind_application_library (environment);
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
