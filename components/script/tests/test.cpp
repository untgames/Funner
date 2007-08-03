#include <stdio.h>
#include <script/lua.h>
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
  int element;
};

void my_func2 (int arg1, size_t arg2, float arg3, double arg4, const char* arg5)
{
  printf ("Arg1 = %d\n", arg1);
  printf ("Arg2 = %u\n", arg2);
  printf ("Arg3 = %f\n", arg3);
  printf ("Arg4 = %f\n", arg4);
  printf ("Arg5 = %s\n", arg5);
}

float my_func3 (MyStruct* arg)
{
//  printf ("Arg = %p\n", arg);
  return 39.84f;
}

void DebugLogFunction (const char* message)
{
  printf ("%s\n", message);
}

int main ()
{
  try
  {
    Environment script, script2;
    
    MyStruct* my_struct = new MyStruct;

    printf ("Results of lua test:\n\n");
    printf ("Testing of binding and calling C function from Lua:\n");
    script.RegisterFunction <int (const char*)> ("my_c_func", &my_func);
    script.RegisterFunction <void (int, size_t, float, double, const char*)> ("my_c_func2", &my_func2);
    script.RegisterFunction <float (MyStruct*)> ("my_c_func3", &my_func3);
    script.DoFile (file_name2, &DebugLogFunction);

    printf ("\nTesting of calling Lua function from C:\n");
    printf ("Lua call result: %d\n", script.Invoke ("my_lua", 7, Result<int> ()));
    script.Invoke ("my_lua", 12);
    script.Invoke ("my_lua2");
    
    printf ("\nTesting of calling Lua function which is binded C function from C:\n");
    printf ("Lua call of binded function result: %f\n", script.Invoke<float> ("my_c_func3", my_struct));

    if (script.HasFunction ("my_c_func"))
      printf ("my_c_func founded\n");
    else
      printf ("my_c_func not founded\n");

    if (script.HasFunction ("my_c_func15"))
      printf ("my_c_func15 founded\n");
    else
      printf ("my_c_func15 not founded\n");

    if (script.HasFunction ("my_lua"))
      printf ("my_lua founded\n");
    else
      printf ("my_lua not founded\n");

    delete my_struct;
  }
  catch (std::exception& exception)
  {      
    printf ("exception: %s\n",exception.what ());
    return 1;
  }                                               
  catch (char* exception)
  {
    printf ("%s", exception);
    return 1;
  }
  catch (...)
  {
    printf ("unknown exception\n");
    return 1;
  }

  return 0;
}