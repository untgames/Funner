#include "shared.h"

using namespace script;

const char* lua_f = "function f (arg1) return arg1*arg1 end";

void hello_func ()
{
  printf ("Hello world!\n");
}

int main ()
{
  try
  {
    printf ("Results of lua_interpreter_info_test:\n");
    
    xtl::shared_ptr<Environment> env (new Environment);
    
    Invoker invoker = make_invoker (hello_func);
    
    InvokerRegistry& registry = env->CreateLibrary ("global");

    registry.Register ("f1", invoker);

    Shell shell ("lua", env);
    
    xtl::com_ptr<IInterpreter> interpreter (shell.Interpreter ());

    printf ("Interpreter name - %s\n", interpreter->Name ());
    printf ("Interpreter has function 'f1': %d\n", interpreter->HasFunction ("f1"));
    printf ("Unregister 'f1'\n");

    registry.Unregister ("f1");

    printf ("Interpreter has function 'f1': %d\n", interpreter->HasFunction ("f1"));

    interpreter->DoCommands ("lua_f", lua_f, strlen (lua_f));

    printf ("Interpreter has function 'f': %d\n", interpreter->HasFunction ("f"));

    printf ("invoke result: %g\n", invoke<float> (*interpreter, "f", 3.0f));
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
