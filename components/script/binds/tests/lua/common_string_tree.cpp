#include "shared.h"

const char* SCRIPT_FILE_NAME = "data/common_string_tree.lua";

void print_indent (size_t indent_size)
{
  for (size_t i = 0; i < indent_size; i++)
    printf (" ");
}

void log_handler (const char* log_name, const char* event)
{
  printf ("Log event from log '%s': '%s'\n", log_name, event);
}

int main ()
{
  printf ("Results of common_string_tree_test:\n");
  
  try
  {
    xtl::auto_connection log_connection = common::LogSystem::RegisterLogHandler ("*", &log_handler);

    xtl::shared_ptr<Environment> env (new Environment);
    
    InvokerRegistry& lib = env->CreateLibrary ("Utils");

    lib.Register ("PrintIndent", make_invoker (&print_indent));

    Shell shell ("lua", env);

    xtl::com_ptr<IInterpreter> script (shell.Interpreter ());                
  
    bind_common_string_tree (*env);

    load_script (*script, SCRIPT_FILE_NAME);
    
    printf ("Test library:\n");

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
