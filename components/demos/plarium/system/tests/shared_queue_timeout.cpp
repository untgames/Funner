#include "shared.h"

using namespace plarium::system;
using namespace plarium::utility;

typedef SharedQueue<int> TestQueue;

TestQueue* queue = 0;

void* run (void*)
{
  printf ("thread started\n");

  std::auto_ptr<int> obj1 = queue->Dequeue (1000);

  if (obj1.get ())
    printf ("Object %d dequeued\n", *obj1);
  else
    printf ("Dequeue failed\n");

  std::auto_ptr<int> obj2 = queue->Dequeue (500);

  if (obj2.get  ())
    printf ("Object %d dequeued\n", *obj2);
  else
    printf ("Dequeue failed\n");

  return 0;
}

int main ()
{
  queue = new TestQueue (1);

  Thread thread (&run);

  std::auto_ptr<int> obj1 (new int (5));

  queue->Enqueue (obj1);

  obj1.release ();

  thread.Join ();

  return 0;
}
