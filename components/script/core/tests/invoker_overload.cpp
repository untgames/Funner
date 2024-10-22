#include "shared.h"

const char* f (float x, const char* y)
{
  printf ("f(%g,%s)\n", x, y);

  return "f (float, const char*)";
}

const char* f (float x, int y)
{
  printf ("f(%g,%d)\n", x, y);

  return "f (float, int)";
}

const char* f (float x, const char* y, int z)
{
  printf ("f(%g,%s,%d)\n", x, y, z);

  return "f (float, const char*, int)";
}

int main ()
{
  printf ("Results of invoker_overload_test:\n");

  try
  {
    Invoker f1 = make_invoker (make_invoker (xtl::implicit_cast<const char* (*)(float, const char*, int)> (&f)),
                               make_invoker (xtl::implicit_cast<const char* (*)(float, const char*)> (&f)),
                               make_invoker (xtl::implicit_cast<const char* (*)(float, int)> (&f)));

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
    stack.Push ((ptrdiff_t)2);

    f1 (stack);
    stack.Dump ();

    stack.Clear ();
    stack.PushSymbol ("dummy");
    stack.Push (3.0f);
    stack.Push ((ptrdiff_t)2);

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
