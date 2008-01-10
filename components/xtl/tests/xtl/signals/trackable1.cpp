#include "test.h"

void handler ()
{
  printf ("handler\n");
}

int main ()
{
  printf ("Results of trackable1_test:\n");  

  trackable t;

  t.connect_tracker (&handler);

  printf ("before exit\n");

  return 0;
}
