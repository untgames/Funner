#include "shared.h"

const char* SCRIPT_FILE_NAME = "data/mathlib.lua";

int main ()
{
  printf ("Results of mathlib1_test:\n");
  
  try
  {
    xtl::shared_ptr<Environment> env    (new Environment);
    xtl::com_ptr<IInterpreter>   script (create_lua_interpreter (env));
    
    env->Library ("global").Register ("typename", make_invoker (&get_typename));
  
    bind_math_library (*env);
    load_script       (*script, SCRIPT_FILE_NAME);
    
/*    printf ("Test vec3f library:\n");

    invoke<void> (*script, "test_vector", vec3f (1, 2, 3));
    
    printf ("Test mat4f library:\n");
    
    invoke<void> (*script, "test_matrix", mat4f (1));*/
    
    printf ("Test quatf library:\n");
    
    invoke<void> (*script, "test_quat", quatf (1));
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
