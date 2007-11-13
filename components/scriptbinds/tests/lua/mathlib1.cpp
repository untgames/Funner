#include "shared.h"

const char* SCRIPT_FILE_NAME = "data/mathlib.lua";

int main ()
{
  printf ("Results of mathlib1_test:\n");
  
  try
  {
    xtl::shared_ptr<Environment> env    (new Environment);
    xtl::com_ptr<IInterpreter>   script (create_lua_interpreter (env));
  
    bind_math_library (*env);
    do_script<void>   (*script, SCRIPT_FILE_NAME, vec3f (1, 2, 3));
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
