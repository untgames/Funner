#include "shared.h"

int main ()
{
  printf ("Results of callback_shell_test:\n");

  try
  {
    Invoker f1 = make_shell_callback_invoker ();

    MyInterpreter interpreter;

    typedef xtl::function<void (const char*)> my_fn;

    interpreter.Stack ().PushSymbol ("dummy");
    
    f1 (interpreter.Stack ());

    my_fn f2 = xtl::any_multicast<my_fn> (interpreter.Stack ().GetVariant (interpreter.Stack ().Size () - 1));

    printf ("call f2\n");

    f2 ("Hello world");    
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
