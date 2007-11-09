#include <stdio.h>
#include <xtl/iterator.h>
#include <script/bind.h>

using namespace script;

const char* lua_f = "function f (arg1)  print (arg1)  return arg1*arg1 end";

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
    
    InvokerRegistry invoker_registry;
    Invoker         invoker  = make_invoker (hello_func);

    InvokerRegistry::Iterator i = invoker_registry.CreateIterator ();

    invoker_registry.Register ("f1", invoker);

    xtl::com_ptr<IInterpreter> interpreter (create_lua_interpreter (invoker_registry));

    printf ("Interpreter name - %s\n", interpreter->Name ());

    printf ("Interpreter has function 'f1': %d\n", interpreter->HasFunction ("f1"));

    interpreter->DoCommands ("lua_f", lua_f, strlen (lua_f), log_function);

    printf ("Interpreter has function 'f': %d\n", interpreter->HasFunction ("f"));

    interpreter->Stack ().Push ("f");

    interpreter->Invoke (0, 0);
  }
  catch (std::exception& exception)
  {      
    printf ("exception: %s\n",exception.what ());
    return 1;
  }                                               
  catch (...)
  {
    printf ("unknown exception\n");
    return 1;
  }

  return 0;
}