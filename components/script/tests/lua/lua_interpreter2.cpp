#include <stdio.h>
#include <xtl/iterator.h>
#include <xtl/shared_ptr.h>
#include <script/bind.h>
#include <script/environment.h>

using namespace script;

const char* lua_f = "function test (object) return object:f(2) end";

struct A
{
  A (int in_id) : id (in_id) {}
  
  int id;
};

float f (const A& object, int x)
{
  printf ("f(A(%d),%d)\n", object.id, x);
  return 3.0f;
}

void log_function (const char* s)
{
  printf ("%s\n", s);
}

int main ()
{
  try
  {
    printf ("Results of lua_interpreter test:\n");
    
    xtl::shared_ptr<Environment> env (new Environment);
    
    Invoker invoker = make_invoker (&f);
    
    InvokerRegistry&           registry (env->CreateRegistry (typeid(A).name ()));
    xtl::com_ptr<IInterpreter> interpreter (create_lua_interpreter (env));    

    registry.Register ("f", invoker);

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
