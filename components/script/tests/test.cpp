#include <stdio.h>
#include <script.h>
#include <common/exception.h>

using namespace scriptlib;
using namespace common;

const char* file_name = "data/test1.lua";

int main ()
{
  try
  {
    ScriptEnv script (ST_LUA);

    script.Load (file_name);

    printf ("Value of 'a' = '%s'\n", script.GetVariable <const char*> ("a"));
    script.SetVariable ("a", "24"); 
    printf ("New value of 'a' = '%s'\n", script.GetVariable <const char*> ("a"));
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