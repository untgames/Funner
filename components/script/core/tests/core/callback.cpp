#include "shared.h"

int main ()
{
  try
  {
    Invoker f1 = make_callback_invoker<const char* (int, float, const char*)> ();

    MyInterpreter interpreter;

    typedef xtl::function<const char* (int, float, const char*)> my_fn;

    interpreter.Stack ().PushSymbol ("dummy");
    interpreter.Stack ().Push ("my_callback");
    
    f1 (interpreter.Stack ());

    my_fn f2 = xtl::any_multicast<my_fn> (interpreter.Stack ().GetVariant (interpreter.Stack ().Size () - 1));
    
    printf ("call f2\n");
    
    printf ("  result: '%s'\n", f2 (2, 3.14f, "Hello world"));
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
