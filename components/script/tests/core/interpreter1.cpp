#include "shared.h"

int main ()
{
  printf ("Results of interpreter1_test:\n");
  
  try
  {
    MyInterpreter interpreter;
    
    invoke<void> (interpreter, "f1", 1, 2.0f, "Hello world");
    
    printf ("result: '%s'\n", invoke<const char*> (interpreter, "f2", xtl::shared_ptr<X> (new Y), xtl::ref (A ("A1"))));
  }
  catch (xtl::bad_any_cast& exception)
  {
    printf ("%s: '%s' -> '%s'\n", exception.what (), exception.source_type ().name (), exception.target_type ().name ());
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }
  
  return 0;
}
