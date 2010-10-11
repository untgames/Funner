#include "shared.h"

const char* SCRIPT_FILE_NAME = "data/sg.lua";

int main ()
{
  printf ("Results of sg_test:\n");
  
  try
  {
    Environment env;
    
    Shell shell ("lua", env);

    xtl::com_ptr<IInterpreter> script (shell.Interpreter ());                
    
    env.BindLibraries ("BoundVolumes");
    env.BindLibraries ("Math");
    env.BindLibraries ("SceneGraph");
  
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
