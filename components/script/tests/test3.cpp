#include <stdio.h>
#include <script.h>

using namespace script::lua;
using namespace common;

const char* file_name = "data/test1.lua";

struct A
{
  A () { count++; }
  ~A () { count--; }
  A (const A&) { count++; }
  
  static int count;
};

int A::count = 0;

int main ()
{
  printf ("Results of test3:\n");

  try
  {
    Environment env;
    Stack* stack  = env.Stack ();

    for (size_t i = 0; i < 10; i++)
      stack->Push (A ());

    stack->Pop (10);
    
    printf ("count=%u\n", A::count);
  }
  catch (std::exception& exception)
  {      
    printf ("exception: %s\n",exception.what ());
    return 1;
  }                                               
  catch (...)
  {
    printf ("unknown exception\n");
    return 1;
  }

  return 0;
}
