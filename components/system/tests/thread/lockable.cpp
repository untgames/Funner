#include "shared.h"

Lockable* mutex = 0;
bool      flag  = false;

int run ()
{
  printf ("another thread: lock mutex\n");
  
  flag = true;

  mutex->Lock ();
    
  printf ("another thread: mutex locked\n");
  
  return 0;
}

int main ()
{
  printf ("Results of lockable_test:\n");

  try    
  {
    LogFilter filter ("common.threads.*", &print_log);
    
    printf ("create mutex\n");
    
    Lockable mutex_holder;
    
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
