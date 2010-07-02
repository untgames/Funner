//тестиоование функций вставки / извлечения элементов из очереди
#include <stl/queue>
#include <stdio.h>

using namespace stl;

int main ()
{
  printf ("Results of queue04_test:\n");

  queue <int> q1;

  q1.push (10);
  q1.push (20);
  q1.push (30);

  printf ("The queue length is %lu\n", q1.size ());

  printf ("back of queue q1 is %d\n", q1.back ());
  printf ("front of queue q1 is %d\n", q1.front ());

  printf ("Unqueue q1: ");
  while (!q1.empty ())
  {
    printf ("%d ", q1.front ());
    q1.pop ();
  }

  printf ("\nThe queue length is %lu\n", q1.size ());

  return 0;
}

