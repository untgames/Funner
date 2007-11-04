#include "shared.h"

void invoke (const char* name, const Invoker& f, IStack& stack)
{
  printf ("invoke '%s':\n", name);

  try
  {
    f (stack);    
  }
  catch (xtl::bad_any_cast& exception)
  {
    printf ("failed, exception %s: '%s' -> '%s'\n", exception.what (), exception.source_type ().name (), exception.target_type ().name ());
  }  
  catch (std::exception& exception)
  {
    printf ("failed, exception: %s\n", exception.what ());
  }
}

int main ()
{
  printf ("Results of invoker2_test:\n");
  
  try
  {
    Invoker f1 = make_invoker (&A::f), f2 = make_invoker (&X::test);
    MyStack stack;
    
    A a ("A1");
    
    stack.Push (xtl::ref (a));
    stack.Push (3.0f);
    stack.Push ("Hello world");

    invoke ("f1", f1, stack);
    
    stack.Dump ();

    printf ("a.name='%s'\n", a.name.c_str ());
    
    stack.Pop  (stack.Size ());
    stack.Push (xtl::shared_ptr<X> (new Y));
    
    invoke ("f2", f2, stack);    

    stack.Dump ();
    
    stack.Pop  (stack.Size ());
    stack.Push (xtl::shared_ptr<const X> (new Y));

    invoke ("f2", f2, stack);
    
    stack.Dump ();
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
