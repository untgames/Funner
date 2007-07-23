#include <stdio.h>
#include <script.h>
#include <common/exception.h>
#include <xtl/functional>

using namespace scriptlib;
using namespace common;

const char* file_name = "data/test1.lua";
const char* file_name2 = "data/test2.lua";

void my_func (const char* arg)
{
  printf ("My function work!!!\n");
}

int main ()
{
  try
  {
    ScriptEnv script (ST_LUA), script2 (ST_LUA);

    script.BindFunction ("my_c_func", &my_func);
    script.Load (file_name2);
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