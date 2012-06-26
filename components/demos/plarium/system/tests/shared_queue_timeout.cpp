#include "shared.h"

using namespace plarium::system;
using namespace plarium::utility;

typedef SharedQueue<int> TestQueue;

TestQueue* queue = 0;

void* run (void*)
{
  printf ("thread started\n");

  sgi_stl::auto_ptr<int> obj1;

  queue->Dequeue (1000, obj1);

  if (obj1.get ())
    printf ("Object %d dequeued\n", *obj1);
  else
    printf ("Dequeue failed\n");

  sgi_stl::auto_ptr<int> obj2;

  queue->Dequeue (500, obj2);

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

  sgi_stl::auto_ptr<int> obj1 (new int (5));

  queue->Enqueue (obj1);

  obj1.release ();

  thread.Join ();

  return 0;
}
