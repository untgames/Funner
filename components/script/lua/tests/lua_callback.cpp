#include "shared.h"

using namespace script;

const char* lua_f = 
"function callback(x)\n"
"  print('x=' .. tostring (x))\n"
"  return tostring (x)\n"
"end\n"
"function test ()\n"
"  return EventHandler ('callback')\n"
"end\n"
;

int main ()
{
  try
  {
    printf ("Results of lua_callback_test:\n");
    
    typedef xtl::function<const char* (int)> my_fn;

    my_fn fn;
    
    {    
      Environment env;

      InvokerRegistry& registry = env.CreateLibrary ("global");

      Shell shell ("lua", env);

      xtl::com_ptr<IInterpreter> interpreter (shell.Interpreter ());

      registry.Register ("EventHandler", make_callback_invoker<const char* (int)> ());

      interpreter->DoCommands ("lua_f", lua_f, strlen (lua_f));

      typedef xtl::function<const char* (int)> my_fn;

      fn = invoke<my_fn> (*interpreter, "test");
      
      printf ("result: '%s'\n", fn (3));
    }

    fn (12); //exception must be raised
  }
  catch (std::exception& exception)
  {      
    printf ("exception: %s\n",exception.what ());
  }
  catch (...)
  {
    printf ("unknown exception\n");
  }

  return 0;
}
