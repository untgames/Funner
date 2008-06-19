#include "shared.h"

using namespace script;

const char* lua_f = 
"function test ()\nMyLibrary.Id = MyLibrary.Id1\nprint (\"Test MyLibrary.Id: \" .. MyLibrary.Id)\nend\n"
;

int my_id = 3;

int get_id ()
{
  return my_id;
}

void set_id (int id)
{
  my_id = id;
}

void log_function (const char* s)
{
  printf ("%s\n", s);
}

int main ()
{
  try
  {
    printf ("Results of lua_interpreter5_test:\n");
    
    xtl::shared_ptr<Environment> env (new Environment);

    InvokerRegistry& registry = env->CreateLibrary ("static.MyLibrary");

    Shell shell ("lua", env);
    
    xtl::com_ptr<IInterpreter> interpreter (shell.Interpreter ());

    registry.Register ("get_Id", make_invoker (&get_id));
    registry.Register ("set_Id", make_invoker (&set_id));
    registry.Register ("get_Id1", make_const (12));

    interpreter->DoCommands ("lua_f", lua_f, strlen (lua_f), log_function);

    printf ("invoke\n");

    invoke<void> (*interpreter, "test");
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
