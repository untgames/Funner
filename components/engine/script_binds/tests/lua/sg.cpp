#include "shared.h"

const char* SCRIPT_FILE_NAME = "data/sg.lua";

void log_handler (const char* log_name, const char* event)
{
  printf ("%s: %s\n", log_name, event);
}

int main ()
{
  printf ("Results of sg_test:\n");
  
  try
  {
    common::LogFilter filter ("scene_graph.*", &log_handler);

    Environment env;
    
    Shell shell ("lua", env);

    xtl::com_ptr<IInterpreter> script (shell.Interpreter ());                
    
    env.BindLibraries ("BoundVolumes");
    env.BindLibraries ("Common");
    env.BindLibraries ("Math");
    env.BindLibraries ("SceneGraph");
    env.BindLibraries ("SceneInput");
    env.BindLibraries ("MediaFont");
  
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
