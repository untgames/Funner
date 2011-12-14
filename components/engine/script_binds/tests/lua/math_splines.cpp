#include "shared.h"

const char* SCRIPT_FILE_NAME = "data/math_splines.lua";

int main ()
{
  printf ("Results of math_splines1_test:\n");
  
  try
  {
    Environment env;
    
    Shell shell ("lua", env);

    xtl::com_ptr<IInterpreter> script (shell.Interpreter ());            

    env.Library ("global").Register ("typename", make_invoker (&get_typename));
  
    env.BindLibraries ("Math");
    load_script       (*script, SCRIPT_FILE_NAME);
    
    printf ("Test splines library:\n");
    
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
