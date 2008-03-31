#include "shared.h"

char* f (float x, const char* y)
{
  printf ("f(%g,%s)\n", x, y);

  return "f (float, const char*)";
}

char* f (float x, int y)
{
  printf ("f(%g,%d)\n", x, y);

  return "f (float, int)";
}

char* f (float x, const char* y, int z)
{
  printf ("f(%g,%s,%d)\n", x, y, z);

  return "f (float, const char*, int)";
}

int main ()
{
  printf ("Results of invoker_overload_test:\n");
  
  try
  {
    Invoker f1 = make_invoker (make_overload (xtl::implicit_cast<char* (*)(float, const char*, int)> (&f)),
                               make_overload (xtl::implicit_cast<char* (*)(float, const char*)> (&f)),
                               make_overload (xtl::implicit_cast<char* (*)(float, int)> (&f)));

    MyStack stack;

    stack.PushSymbol ("dummy");
    stack.Push (3.0f);
    stack.Push ("Hello world");

    f1 (stack);
    stack.Dump ();
    
    stack.Clear ();
    stack.PushSymbol ("dummy");
    stack.Push (3.0f);
    stack.Push ("Hello world");    
    stack.Push (2);
    
    f1 (stack);
    stack.Dump ();

    stack.Clear ();
    stack.PushSymbol ("dummy");
    stack.Push (3.0f);
    stack.Push (2);

    f1 (stack);
    stack.Dump ();
    
    stack.Clear ();
    stack.PushSymbol ("dummy");
    stack.Push ("Hello world");
    stack.Push ("3");

    f1 (stack);
    stack.Dump ();
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
