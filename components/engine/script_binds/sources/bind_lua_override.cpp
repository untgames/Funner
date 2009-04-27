#include "shared.h"

using namespace script;

namespace
{

/*
    Константы
*/

const char* COMPONENT_NAME = "script.binds.LuaOverride";
const char* BINDER_NAME    = "LuaOverride";

/*
    Функции
*/

void print_override (const char* message)
{
  common::Console::Printf ("%s\n", message);
}

void error_override (const char* message)
{
  throw xtl::format_operation_exception ("", message ? "error: %s" : "error", message);
}

void assert_override (bool assertion, const char* message)
{
  if (assertion)
    return;
    
  error_override (message ? message : "assertion failed");
}

void assert_override (bool assertion)
{
  assert_override (assertion, 0);
}

struct DoFileOverride
{
  size_t operator () (IStack& stack)
  {
    size_t initial_stack_size = stack.Size ();

    const char* file_name = stack.GetString (1);

    common::InputFile input_file (file_name);

    xtl::uninitialized_storage<char> file_content (input_file.Size ());

    input_file.Read (file_content.data (), file_content.size ());

    stack.Interpreter ().DoCommands (file_name, file_content.data (), file_content.size ());

    return stack.Size () - initial_stack_size;
  }
};

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
      InvokerRegistry& lib = environment.Library ("global");

        //регистрация операций

      lib.Register ("print",  make_invoker (&print_override));
      lib.Register ("error",  make_invoker (&error_override));
      lib.Register ("assert", make_invoker (
                                make_invoker ((void (*)(bool, const char*))&assert_override),
                                make_invoker ((void (*)(bool))&assert_override)));
      lib.Register ("dofile", DoFileOverride ());
    }
};

extern "C"
{

common::ComponentRegistrator<Component> LuaOverrideScriptBind (COMPONENT_NAME);

}

}
