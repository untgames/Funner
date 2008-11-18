#include "shared.h"

const char* SCRIPT_FILE_NAME = "data/lua_override.lua";

void console_handler (const char* message)
{
  printf ("%s", message);
}

int main ()
{
  common::Console::RegisterEventHandler (&console_handler);

  printf ("Results of lua_override_test:\n");
  
  try
  {
    xtl::shared_ptr<Environment> env (new Environment);
    
    Shell shell ("lua", env);

    xtl::com_ptr<IInterpreter> script (shell.Interpreter ());                
    
    env->BindLibraries ("LuaOverride");

    load_script (*script, SCRIPT_FILE_NAME);

    invoke<void> (*script, "test");
  }
  catch (xtl::bad_any_cast& exception)
  {
    printf ("%s: %s -> %s\n", exception.what (), exception.source_type ().name (), exception.target_type ().name ());
  }    
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
