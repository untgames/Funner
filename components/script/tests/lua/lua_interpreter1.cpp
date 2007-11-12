#include <stdio.h>
#include <xtl/iterator.h>
#include <xtl/shared_ptr.h>
#include <script/bind.h>
#include <script/environment.h>

using namespace script;

const char* lua_f = "function f (arg1) return arg1*arg1 end";

void hello_func ()
{
  printf ("Hello world!\n");
}

void log_function (const char* msg)
{
  printf ("%s\n", msg);
}

int main ()
{
  try
  {
    printf ("Results of lua_interpreter test:\n");
    
    xtl::shared_ptr<Environment> env (new Environment);
    
    Invoker invoker = make_invoker (hello_func);
    
    InvokerRegistry& registry = env->CreateRegistry ("global");

    registry.Register ("f1", invoker);

    xtl::com_ptr<IInterpreter> interpreter (create_lua_interpreter (env));

    printf ("Interpreter name - %s\n", interpreter->Name ());
    printf ("Interpreter has function 'f1': %d\n", interpreter->HasFunction ("f1"));
    printf ("Unregister 'f1'\n");

    registry.Unregister ("f1");

    printf ("Interpreter has function 'f1': %d\n", interpreter->HasFunction ("f1"));

    interpreter->DoCommands ("lua_f", lua_f, strlen (lua_f), log_function);

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
