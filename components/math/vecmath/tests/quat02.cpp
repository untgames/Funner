#include "shared.h"

int main ()
{
  printf ("Results of quat02_test:\n");
  
  quatf q (1.0f, 2.0f, 3.0f, 4.0f);

  dump ("+q", +q);
  dump ("-q", -q);

  return 0;
}
