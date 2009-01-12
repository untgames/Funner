#include "shared.h"

Mutex*        mutex = 0;
volatile bool flag  = false;

int run ()
{
  printf ("another thread: lock mutex\n");

  printf ("another thread: try lock mutex: %d\n", mutex->TryLock ());

  flag = true;

  mutex->Lock ();

  printf ("another thread: mutex locked\n");

  return 0;
}

int main ()
{
  printf ("Results of mutex_test:\n");

  try
  {
    LogFilter filter ("system.threads.*", &print_log);

    printf ("create mutex\n");

    Mutex mutex_holder;

    mutex = &mutex_holder;

    printf ("lock mutex\n");

    mutex->Lock ();

    printf ("create thread\n");

    Thread thread (&run);

    while (!flag);

    printf ("unlock mutex\n");

    mutex->Unlock ();

    thread.Join ();

    printf ("exit\n");
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
