#include "shared.h"

using namespace script;

const int BAD_VALUE = 12;

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
  buffer = common::format ("A(%d)", value.id);
}

int get_id (const A& a)
{
  return a.id;
}

int set_id (A& a, int id)
{
  if (id == BAD_VALUE)
   throw xtl::make_argument_exception ("set_id(A)", "id", id);

  return a.id = id;
}

int main ()
{
  try
  {
    printf ("Results of lua_properties_test:\n");
    
    Environment env;
    
    InvokerRegistry registry = env.CreateLibrary ("my_library");

    env.RegisterType<A> ("my_library");

    Shell shell ("lua", env);
    
    xtl::com_ptr<IInterpreter> interpreter (shell.Interpreter ());

    registry.Register ("get_id", make_invoker (&get_id));
    registry.Register ("set_id", make_invoker (&set_id));
    
    interpreter->DoCommands ("lua_f", lua_f, strlen (lua_f));

    printf ("invoke result: %d\n", invoke<int> (*interpreter, "test1", A (4)));
    
    A a;
    
    invoke<void> (*interpreter, "test2", xtl::ref (a), 2);
    
    printf ("after invoke: A(%d)\n", a.id);
    
    invoke<void> (*interpreter, "test2", xtl::ref (a), BAD_VALUE);
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
