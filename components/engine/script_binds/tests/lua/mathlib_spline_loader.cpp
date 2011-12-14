#include "shared.h"

const char* SCRIPT_FILE_NAME = "data/mathlib_spline_loader.lua";

int main ()
{
  printf ("Results of mathlib_spline_loader_test:\n");
  
  try
  {
    Environment env;
    
    Shell shell ("lua", env);

    xtl::com_ptr<IInterpreter> script (shell.Interpreter ());            

    env.Library ("global").Register ("typename", make_invoker (&get_typename));
  
    env.BindLibraries ("Common");
    env.BindLibraries ("Math");
    env.BindLibraries ("MathSplineLoader");
    load_script       (*script, SCRIPT_FILE_NAME);
    
    invoke<void> (*script, "test");
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
