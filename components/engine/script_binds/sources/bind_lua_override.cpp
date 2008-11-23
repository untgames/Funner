#include "shared.h"

using namespace script;

namespace
{

/*
    Константы
*/

const char* COMPONENT_NAME = "script.binds.LuaOverride";
const char* BINDER_NAME    = "LuaOverride";
const char* LOG_NAME       = COMPONENT_NAME;

struct ShellLogHandler
{
  ShellLogHandler (common::Log& in_log) : log (in_log), was_error (false) {}

  void operator () (const char* message)
  {
    was_error = true;

    log.Print (message);
  }

  common::Log& log;
  bool         was_error;
};

void print_override (const char* message)
{
  common::Console::Printf ("%s\n", message);
}

struct DoFileOverride
{
  DoFileOverride () : log (LOG_NAME) {}

  size_t operator () (IStack& stack)
  {
    size_t initial_stack_size = stack.Size ();

    const char* file_name = stack.GetString (1);

    common::InputFile input_file (file_name);

    xtl::uninitialized_storage<char> file_content (input_file.Size ());

    input_file.Read (file_content.data (), file_content.size ());

    ShellLogHandler log_handler (log);

    stack.Interpreter ().DoCommands (file_name, file_content.data (), file_content.size (), xtl::ref (log_handler));

    if (log_handler.was_error)
      throw xtl::format_operation_exception ("script::binds::LuaOverride::do_file", "Lua exception while dofile '%s'", file_name);

    return stack.Size () - initial_stack_size;
  }

  common::Log log;
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
      lib.Register ("dofile", DoFileOverride ());
    }
};

extern "C"
{

common::ComponentRegistrator<Component> LuaOverrideScriptBind (COMPONENT_NAME);

}

}
