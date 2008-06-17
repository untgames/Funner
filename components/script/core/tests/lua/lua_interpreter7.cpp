#include "shared.h"

using namespace script;

const char* lua_f = 
"function test ()\nif (f ()) then\n print(\"not null\")\nelse print (\"null\")\nend\nend\n"
;

struct A
{
  public:
    virtual ~A () {}
};

struct B: A
{
  public:
    virtual ~B () {}
};

typedef xtl::shared_ptr<A> Ptr;

Ptr f ()
{
  return Ptr ();
}

void log_function (const char* s)
{
  printf ("%s\n", s);
}

int main ()
{
  try
  {
    printf ("Results of lua_interpreter7_test:\n");
    
    xtl::shared_ptr<Environment> env (new Environment);

    InvokerRegistry& registry = env->CreateLibrary ("global");

    xtl::com_ptr<IInterpreter> interpreter (create_lua_interpreter (env));

    registry.Register ("f", make_invoker (&f));

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
