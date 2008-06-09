#include "shared.h"

struct test_exception_tag;

typedef derived_exception<test_exception_tag> test_exception;

template <int I>
void f ()
{
  try
  {
    f<I-1> ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("::f%d", I);
    throw;
  }
}

template <> void f<0> ()
{
  throw test_exception ("::f0", "test exception message");
}

int main ()
{
  printf ("Results of message_exception_test:\n");
  
  try
  {
    f<10> ();
  }
  catch (std::exception& e)
  {
    printf ("exception: %s\n", e.what ());
  }

  return 0;
}
