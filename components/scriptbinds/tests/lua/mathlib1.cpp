#include "shared.h"

const char* test_buffer =  
"function test_identity (v1)\nreturn v1\nend\n"
"function test_length (v1)\nreturn vecLength (v1)\nend\n"
"function test_qlength (v1)\nreturn vecQLength (v1)\nend\n"
"function test_dot (v1,v2)\nreturn vecDot (v1,v2)\nend\n"
"function test_abs (v1)\nreturn vecAbs (v1)\nend\n"
"function test_min (v1,v2)\nreturn vecMin (v1,v2)\nend\n"
"function test_max (v1,v2)\nreturn vecMax (v1,v2)\nend\n"
"function test_add (v1,v2)\nreturn v1 + v2\nend\n";

int main ()
{
  printf ("Results of mathlib1_test:\n");
  
  try
  {
    xtl::shared_ptr<Environment> env (new Environment);
    xtl::com_ptr<IInterpreter>  script = create_lua_interpreter (env);
  
    bind_math_library (*env);
    
    script->DoCommands ("test_buffer", test_buffer, strlen (test_buffer), &log_print);
    
    print ("test_identity", invoke<vec3f> (*script, "test_identity", vec3f (1, 2, 3)));
    print ("test_length",   invoke<float> (*script, "test_length", vec3f (0, 0, 2)));
    print ("test_qlength",  invoke<float> (*script, "test_qlength", vec3f (1, 2, 3)));
    
    invoke<void>  (*script, "test_add", vec3f (1, 2, 3), vec3f (4, 5, 6));
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
