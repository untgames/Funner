#include "shared.h"

#include <input/cursor.h>

using namespace input;
using namespace script;

namespace
{

/*
    Константы
*/

const char* COMPONENT_NAME = "script.binds.Input";
const char* BINDER_NAME    = "Input";
const char* CURSOR_LIBRARY = "Input.Cursor";

/*
    Регистрация библиотек
*/

Cursor create_cursor ()
{
  return Cursor ();
}

void bind_cursor_library (Environment& environment)
{
  InvokerRegistry& lib = environment.CreateLibrary (CURSOR_LIBRARY);
  
  lib.Register ("Create",       make_invoker (&create_cursor));
  lib.Register ("get_Position", make_invoker (&Cursor::Position));
  lib.Register ("set_Position", make_invoker (xtl::implicit_cast<void (Cursor::*)(const math::vec2f&)> (&Cursor::SetPosition)));
  lib.Register ("SetPosition",  make_invoker (xtl::implicit_cast<void (Cursor::*)(float, float)> (&Cursor::SetPosition)));
  lib.Register ("get_Visible",  make_invoker (&Cursor::IsVisible));
  lib.Register ("set_Visible",  make_invoker (&Cursor::SetVisible));
  lib.Register ("Show",         make_invoker (&Cursor::Show));
  lib.Register ("Hide",         make_invoker (&Cursor::Hide));
  
  environment.RegisterType<Cursor> (CURSOR_LIBRARY);
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
      bind_cursor_library (environment);
    }
};

extern "C"
{

common::ComponentRegistrator<Component> InputScriptBind (COMPONENT_NAME);

}

}
