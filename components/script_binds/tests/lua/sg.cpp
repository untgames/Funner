#include "shared.h"

const char* SCRIPT_FILE_NAME = "data/sg.lua";

int main ()
{
  printf ("Results of sg_test:\n");
  
  try
  {
    xtl::shared_ptr<Environment> env    (new Environment);
    xtl::com_ptr<IInterpreter>   script (create_lua_interpreter (env));
    
    env->Library ("global").Register ("typename", make_invoker (&get_typename));
  
    bind_bv_library   (*env);
    bind_math_library (*env);
    bind_scene_graph_library (*env);
    load_script       (*script, SCRIPT_FILE_NAME);
    
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
