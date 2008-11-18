#include "shared.h"

using namespace script;

namespace
{

/*
    Константы
*/

const char* COMPONENT_NAME = "script.binds.LuaOverride";
const char* BINDER_NAME    = "LuaOverride";

void print_override (const char* message)
{
  common::Console::Printf ("%s\n", message);
}

size_t dofile_override (IStack& stack)
{
  size_t initial_stack_size = stack.Size () - 1;

  const char* file_name = stack.GetString (1);

  stack.Pop (1);

  common::InputFile input_file (file_name);

  xtl::uninitialized_storage<char> file_content (input_file.Size ());

  input_file.Read (file_content.data (), file_content.size ());

  stack.Interpreter ().DoCommands (file_name, file_content.data (), file_content.size (), &common::Console::Print);

  return stack.Size () - initial_stack_size;
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
      InvokerRegistry& lib = environment.Library ("global");

        //регистрация операций

      lib.Register ("print",  make_invoker (&print_override));
      lib.Register ("dofile", &dofile_override);
    }
};

extern "C"
{

common::ComponentRegistrator<Component> LuaOverrideScriptBind (COMPONENT_NAME);

}

}
