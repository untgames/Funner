//тестиоование функций общего назначения
#include <stl/queue>
#include <stdio.h>

using namespace stl;

int main () 
{
  printf ("Results of serg_queue02_test:\n");

  queue <int> q1, q2;

  q1.push (1);

  if (q1.empty ())
     printf ("The queue q1 is empty.\n");
  else
     printf ("The queue q1 is not empty.\n");

  if (q2.empty ())
     printf ("The queue q2 is empty.\n");
  else
     printf ("The queue q2 is not empty.\n");

  printf ("q1.size () = %u\n", q1.size ());
  printf ("q2.size () = %u\n", q2.size ());

  return 0;
}

