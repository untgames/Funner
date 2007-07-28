#include <stdio.h>
#include <script.h>
#include <common/exception.h>
#include <xtl/functional>

using namespace script::lua;
using namespace common;

const char* file_name = "data/test1.lua";
const char* file_name2 = "data/test2.lua";

int my_func (const char* arg)
{
  printf ("My function work!!! Received arg %s\n", arg);
  return 157;
}

struct MyStruct
{
  int item;
};

void my_func2 (int arg1, size_t arg2, float arg3, double arg4, const char* arg5, MyStruct* arg6)
{
  printf ("Arg1 = %d\n", arg1);
  printf ("Arg2 = %u\n", arg2);
  printf ("Arg3 = %f\n", arg3);
  printf ("Arg4 = %f\n", arg4);
  printf ("Arg5 = %s\n", arg5);
  printf ("Arg6 = %p\n", arg6);
}

int main ()
{
  try
  {
    Environment script, script2;
    MyStruct    *my_struct = new MyStruct ();

    my_struct->item = 6734;

    script.BindFunction <int (const char*)> ("my_c_func", &my_func);
    script.BindFunction <void (int, size_t, float, double, const char*, MyStruct*)> ("my_c_func2", &my_func2);
    script.DoFile (file_name2);
    printf ("Lua call result: %d\n", invoke <int, int> (script, "my_lua", 7));
    invoke (script, "my_lua", 12);
    invoke (script, "my_lua2");
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