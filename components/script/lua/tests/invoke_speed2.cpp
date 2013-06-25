#include "shared.h"

using namespace script;

const char* lua_f = "function test () local count = 10000000; for i = 1, count do f () end return count end";

void f ()
{
}

int main ()
{
  try
  {
    printf ("Results of invoke_speed2_test:\n");

    Environment env;

    InvokerRegistry registry = env.CreateLibrary ("global");

    Shell shell ("lua", env);

    xtl::com_ptr<IInterpreter> interpreter (shell.Interpreter ());

    registry.Register ("f", make_invoker (&f));

    interpreter->DoCommands ("lua_f", lua_f, strlen (lua_f));

    size_t start = common::milliseconds ();

    size_t total = invoke<size_t> (*interpreter, "test");

    size_t end = common::milliseconds ();

    printf ("Invokations per second: %.2fM\n", float (total)/float (end-start)*1000.0f / 1000000.0f);
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
