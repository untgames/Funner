#include "shared.h"

const char* SCRIPT_FILE_NAME = "data/mathlib_performance.lua";

float test ()
{
  math::vec3f v (0.f);

  for (size_t i = 1; i < 1000000; i++)
    v += normalize (math::vec3f (i));

  return length (v);
}

int main ()
{
  printf ("Results of mathlib_performance test:\n");
  
  try
  {
    Environment env;
    
    Shell shell ("lua", env);

    xtl::com_ptr<IInterpreter> script (shell.Interpreter ());            

    env.Library ("global").Register ("typename", make_invoker (&get_typename));
  
    env.BindLibraries ("Math");
    load_script       (*script, SCRIPT_FILE_NAME);

    printf ("Test c++ performance:\n");
    
    size_t start_time = common::milliseconds ();

    float test_result = test ();
    
    printf ("Duration = %u ms, result = %f\n", common::milliseconds () - start_time, test_result);
    
    printf ("Test lua performance:\n");

    start_time = common::milliseconds ();
    
    test_result = invoke<float> (*script, "test");
    
    printf ("Duration = %u ms, result = %f\n", common::milliseconds () - start_time, test_result);
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
