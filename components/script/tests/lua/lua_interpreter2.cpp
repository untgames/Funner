#include <stdio.h>
#include <xtl/iterator.h>
#include <xtl/shared_ptr.h>
#include <script/bind.h>
#include <script/environment.h>

using namespace script;

const char* lua_f = "function test (object) return object:f(2) + object:f(3) end";

struct A
{
  A (int in_id) : id (in_id) {}
  
  int id;
};

A f (const A& object, int x)
{
  printf ("f(A(%d),%d)\n", object.id, x);
  return A (object.id + x);
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
    printf ("Results of lua_interpreter2_test:\n");
    
    xtl::shared_ptr<Environment> env (new Environment);
    
    InvokerRegistry& registry = env->CreateLibrary ("my_library");

    env->RegisterType (typeid (A), "my_library");
    env->RegisterType (typeid (const A), "my_library");    

    xtl::com_ptr<IInterpreter> interpreter (create_lua_interpreter (env));

    registry.Register ("f", make_invoker (&f));
    registry.Register ("__add", make_invoker (&my_add));
    
    interpreter->DoCommands ("lua_f", lua_f, strlen (lua_f), log_function);

    printf ("invoke result: %g\n", invoke<float> (*interpreter, "test", A (4)));
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
