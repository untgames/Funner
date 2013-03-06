#include "shared.h"

const char* SCRIPT_FILE_NAME = "data/network.lua";

void log_handler (const char* log_name, const char* event)
{
  printf ("%s: %s\n", log_name, event);
}

void apprun ()
{
  syslib::Application::Run ();
}

int main ()
{
  printf ("Results of network_test:\n");
  
  try
  {
    common::LogFilter filter ("network.*", &log_handler);

    Environment env;

    Shell shell ("lua", env);

    xtl::com_ptr<IInterpreter> script (shell.Interpreter ());

    env.Library ("global").Register ("apprun", make_invoker (&apprun));

    env.BindLibraries ("Common");
    env.BindLibraries ("Network");
    env.BindLibraries ("System");
  
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
  