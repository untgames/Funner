#include "shared.h"

const char* lua_print_name    = "lua_print";
const char* lua_print         = "print (\"Printed by LUA\")";
const char* lua_bad_code_name = "lua_bad_code";
const char* lua_bad_code      = "print (\"Before bad code\")\n lsakdfhlahkfs salhdf sadlfh\n print (\"After bad code\")";

const char* test_file_name     = "data/shell_test1.lua";
const char* bad_test_file_name = "data/shell_test2.lua";
const char* file_list_mask     = "data/shell_list_test*.lua";

using namespace script;

void log_function (const char* s)
{
  printf ("log message: '%s'\n", s);
}

int main ()
{
  try
  {
    printf ("Results of shell1_test:\n");
    
    xtl::shared_ptr<Environment> env (new Environment);
    
    InvokerRegistry& registry = env->CreateLibrary ("my_library");

    Shell shell, lua_shell ("lua", env);

    printf ("shell interpreter name is '%s'\n", shell.InterpreterName ());

    shell = lua_shell;
    
    shell.Execute (lua_print_name, lua_print, strlen (lua_print));

    shell.Swap (Shell ());

    shell.SetInterpreter ("lua", env);

    shell.Execute (lua_print_name, lua_print, strlen (lua_print), &log_function);

    shell.SetInterpreter (shell.Interpreter ());

    shell.Interpreter ()->DoCommands (lua_print_name, lua_print, strlen (lua_print), &log_function);

    printf ("shell interpreter name is '%s'\n", shell.InterpreterName ());

    shell.Execute (lua_bad_code_name, lua_bad_code, strlen (lua_bad_code));
    shell.Execute (lua_bad_code_name, lua_bad_code, strlen (lua_bad_code), &log_function);

    printf ("shell has function 'f' - '%c'\n", shell.HasFunction ("f") ? 'y' : 'n');

    shell.ExecuteFile (test_file_name);

    printf ("shell has function 'f' - '%c'\n", shell.HasFunction ("f") ? 'y' : 'n');

    shell.Execute ("f (\"executing command with this text as arg\")");

    shell.ExecuteFile (bad_test_file_name, &log_function);
    shell.ExecuteFileList (bad_test_file_name, &log_function);

    shell.ExecuteFileList (file_list_mask);
  }
  catch (std::exception& exception)
  {      
    printf ("exception: %s\n",exception.what ());
  }                                               
  catch (...)
  {
    printf ("unknown exception\n");
  }

  return 0;
}
