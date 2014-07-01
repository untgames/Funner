#include "shared.h"

typedef SharedQueue<int> Queue;

int main ()
{
  printf ("Results of shared_queue1:\n");

  try
  {
    Queue q (3);

    q.Push (1);
    q.Push (2);
    q.Push (3);

    printf ("try push: %s\n", q.TryPush (4, 10) ? "true" : "false");
    printf ("queue size: %u\n", q.Size ());
    printf ("max queue size: %u\n", q.MaxSize ());
    printf ("queue empty: %s\n", q.IsEmpty () ? "true" : "false");

    printf ("items:");

    while (!q.IsEmpty ())
      printf (" %d", q.Pop ());

    printf ("\n");

    printf ("try push: %s\n", q.TryPush (4, 10) ? "true" : "false");
  }
  catch (xtl::exception& e)
  {
    printf ("%s\n", e.what ());
  }

  return 0;
}
