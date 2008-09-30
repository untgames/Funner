#include "test.h"

struct Fn
{
  void operator () () const {}
};

void to_string (stl::string& string, const Fn& fn)
{
  string = "special Fn";
}

int main ()
{
  printf ("Results of function_dump_test:\n");
  
  Fn                     functor;
  xtl::function<void ()> fn (functor);

  stl::string buffer;

  to_string (buffer, fn);
  
  printf ("to_string: '%s'\n", buffer.c_str ());

  return 0;
}
