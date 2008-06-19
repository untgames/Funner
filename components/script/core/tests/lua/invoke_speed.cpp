#include "shared.h"

const size_t TOTAL = 100000;

using namespace script;

const char* lua_f = "function test (object) return object:f(2) + object:f(3) end";

struct A
{
  A (int in_id) : id (in_id) {}
  
  int id;
};

A f (const A& object, int x)
{
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
    printf ("Results of invoke_speed_test:\n");
    
    xtl::shared_ptr<Environment> env (new Environment);
    
    InvokerRegistry& registry = env->CreateLibrary ("my_library");

    Shell shell ("lua", env);
    
    xtl::com_ptr<IInterpreter> interpreter (shell.Interpreter ());    

    registry.Register ("f", make_invoker (&f));
    registry.Register ("__add", make_invoker (&my_add));
    env->RegisterType (typeid (A), "my_library");
    
    interpreter->DoCommands ("lua_f", lua_f, strlen (lua_f), log_function);
    
    A value (4);
    
    clock_t start = clock ();
    
    for (size_t i=0; i<TOTAL; i++)
      invoke<float> (*interpreter, "test", value);
      
    clock_t end = clock ();
    
    printf ("Invokations per second: %.2fk\n", float (TOTAL)/float (end-start)*float (CLK_TCK)/1000.0f);
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
