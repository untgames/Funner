#include "shared.h"

const size_t LOOPS_COUNT    = 100000;
const size_t THREADS_COUNT  = 32;
const size_t SPIN_TIME      = 200;
const size_t MAX_QUEUE_SIZE = 1;

typedef SharedQueue<int> Queue;

Queue queue (MAX_QUEUE_SIZE, SPIN_TIME);

int producer ()
{
  for (size_t i=0; i<LOOPS_COUNT; i++)
    queue.Push (i);
 
  return 0;
}

int consumer ()
{
  for (size_t i=0; i<LOOPS_COUNT; i++)
    queue.Pop ();

  return 0;
}

int main ()
{
  printf ("Results of shared_queue2:\n");

  try
  {
    stl::auto_ptr<Thread> threads [THREADS_COUNT * 2];

    for (size_t i=0; i<THREADS_COUNT; i += 2)
    {
      threads [i+0].reset (new Thread (&producer));
      threads [i+1].reset (new Thread (&consumer));
    }

    for (size_t i=0; i<THREADS_COUNT; i++)
      threads [i]->Join ();

    printf ("queue size: %d\n", queue.Size ());
  }
  catch (xtl::exception& e)
  {
    printf ("%s\n", e.what ());
  }

  return 0;
}
