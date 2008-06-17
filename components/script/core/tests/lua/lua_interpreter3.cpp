#include "shared.h"

using namespace script;

const char* lua_f = "function test (object) return print (object:f(2)) end";

struct A
{
  A () : id (-1) {}
  A (int in_id) : id (in_id) {}
  
  int id;
};

void to_string (stl::string& buffer, const A& value)
{
  buffer = common::format ("to_string: A(%d)", value.id);
}

A f (const A& object, int x)
{
  printf ("f(A(%d),%d)\n", object.id, x);
  return A (object.id + x);
}

void log_function (const char* s)
{
  printf ("%s\n", s);
}

int main ()
{
  try
  {
    printf ("Results of lua_interpreter3_test:\n");
    
    xtl::shared_ptr<Environment> env (new Environment);
    
    InvokerRegistry& registry = env->CreateLibrary ("my_library");

    env->RegisterType<A> ("my_library");

    xtl::com_ptr<IInterpreter> interpreter (create_lua_interpreter (env));

    registry.Register ("f", make_invoker (&f));
    
    interpreter->DoCommands ("lua_f", lua_f, strlen (lua_f), log_function);

    invoke<void> (*interpreter, "test", A (4));
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
