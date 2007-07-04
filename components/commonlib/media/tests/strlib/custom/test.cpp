#include <common/strlib.h>
#include <stdio.h>

using namespace common;
using namespace stl;

int main ()
{
  string res = replace ("hello world","(hello) (world)","\\2<->\\1");

  printf ("result='%s'\n",res.c_str ());

  return 0;
}
