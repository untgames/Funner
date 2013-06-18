#include "shared.h"

const char* SCRIPT_FILE_NAME = "data/sg_performance.lua";

int main ()
{
  printf ("Results of sg_performance test:\n");
  
  try
  {
    Environment env;
    
    Shell shell ("lua", env);

    xtl::com_ptr<IInterpreter> script (shell.Interpreter ());            

    env.Library ("global").Register ("typename", make_invoker (&get_typename));
  
    env.BindLibraries ("BoundVolumes");
    env.BindLibraries ("Common");
    env.BindLibraries ("Math");
    env.BindLibraries ("SceneGraph");
    env.BindLibraries ("SceneInput");
    load_script       (*script, SCRIPT_FILE_NAME);
      
    printf ("Test lua performance:\n");

    size_t start_time = common::milliseconds ();
    
    size_t count = invoke<size_t> (*script, "test");
    
    printf ("Per second: %.3fMops\n", double (count) / double (common::milliseconds () - start_time) * 1000.0f / 1000000.0f);
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
