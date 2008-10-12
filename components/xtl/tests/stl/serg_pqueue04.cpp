//тестиоование функций вставки / извлечения элементов из очереди
#include <stdio.h>
#include <stl/queue>

using namespace stl;

int main ()
{
  printf ("Results of serg_pqueue04_test:\n");

  priority_queue <int> q1;

  q1.push (10);
  q1.push (20);
  q1.push (30);

  printf ("The queue length is %lu\n", q1.size ());

  printf ("Unqueue q1: ");
  while (!q1.empty ())
  {
    printf ("%d ", q1.top ());
    q1.pop ();
  }

  printf ("\nThe queue length is %lu\n", q1.size ());

  return 0;
}

