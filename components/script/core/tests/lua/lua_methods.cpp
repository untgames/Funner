#include "shared.h"

using namespace script;

const char* lua_f = "function test1 (object) return object:f(2) + object:f(3) end\n"
"function test2 (object) return print (object:f(2)) end";

struct A
{
  A () : id (-1) {}
  A (int in_id) : id (in_id) {}
  
  int id;
};

A f (const A& object, int x)
{
  printf ("f(A(%d),%d)\n", object.id, x);
  return A (object.id + x);
}

void to_string (stl::string& buffer, const A& value)
{
  buffer = common::format ("to_string: A(%d)", value.id);
}

int my_add (const A& object1, const A& object2)
{
  return object1.id + object2.id;
}

void log_function (const char* s)
{
  printf ("%s\n", s);
}

int main ()
{
  try
  {
    printf ("Results of lua_methods_test:\n");
    
    xtl::shared_ptr<Environment> env (new Environment);
    
    InvokerRegistry& registry = env->CreateLibrary ("my_library");

    env->RegisterType<A> ("my_library");

    Shell shell ("lua", env);
    
    xtl::com_ptr<IInterpreter> interpreter (shell.Interpreter ());

    registry.Register ("f", make_invoker (&f));
    registry.Register ("__add", make_invoker (&my_add));
    
    interpreter->DoCommands ("lua_f", lua_f, strlen (lua_f), log_function);

    printf ("invoke result: %g\n", invoke<float> (*interpreter, "test1", A (4)));

    invoke<void> (*interpreter, "test2", A (4));
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
