#include "shared.h"

using namespace script;

const char* lua_f = 
"function test1 (object)\nreturn object:get_id()+object.id\nend\n"
"function test2 (object, id)\nobject.id = id\nend\n"
;

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

int get_id (const A& a)
{
  return a.id;
}

int set_id (A& a, int id)
{
  if (!id)
   throw xtl::make_argument_exception ("set_id(A)", "id", id);

  return a.id = id;
}

void log_function (const char* s)
{
  printf ("%s\n", s);
}

int main ()
{
  try
  {
    printf ("Results of lua_interpreter4_test:\n");
    
    xtl::shared_ptr<Environment> env (new Environment);
    
    InvokerRegistry& registry = env->CreateLibrary ("my_library");

    env->RegisterType<A> ("my_library");

    xtl::com_ptr<IInterpreter> interpreter (create_lua_interpreter (env));

    registry.Register ("get_id", make_invoker (&get_id));
    registry.Register ("set_id", make_invoker (&set_id));
    
    interpreter->DoCommands ("lua_f", lua_f, strlen (lua_f), log_function);

    printf ("invoke result: %d\n", invoke<int> (*interpreter, "test1", A (4)));
    
    A a;
    
    invoke<void> (*interpreter, "test2", xtl::ref (a), 2);
    
    printf ("after invoke: A(%d)\n", a.id);
    
    invoke<void> (*interpreter, "test2", xtl::ref (a), 0);
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
