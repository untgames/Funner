#include "shared.h"

using namespace plarium::utility;

int main ()
{
  printf ("Results of format_test:\n");
  
  printf ("%s\n", format ("string::format %s, x=%c, y=%d, pi=%.4f","test",'A',12,3.1415f).c_str ());

  return 0;
}
