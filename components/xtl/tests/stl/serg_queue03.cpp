//тестиоование функций доступа
#include <stl/queue>
#include <stl/algorithm>
#include <stdio.h>

using namespace stl;

int main ()
{
  printf ("Results of serg_queue03_test:\n");

  queue <int> q1;

  q1.push (10);
  q1.push (20);
  q1.push (30);

  printf ("The queue length is %lu\n", q1.size ());

  printf ("back of queue q1 is %d\n", q1.back ());
  printf ("front of queue q1 is %d\n", q1.front ());

  swap (q1.back (), q1.front ());
  printf ("After swap (back, front):\n");
  printf ("back of queue q1 is %d\n", q1.back ());
  printf ("front of queue q1 is %d\n", q1.front ());

  return 0;
}

