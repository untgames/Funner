#include "shared.h"

using namespace plarium::system;

Mutex*     mutex = 0;
Condition* condition = 0;

void* run (void*)
{
  mutex->Lock ();
  
  printf ("wait started\n");  
  fflush (stdout);

  condition->Wait (*mutex);

  printf ("wait finished\n");
  fflush (stdout);

  mutex->Unlock ();

  return 0;
}

int main ()
{
  printf ("Results of condition_broadcast_test:\n");

  try
  {
    printf ("create mutex & condition\n");

    Mutex mutex_holder;
    Condition condition_holder;

    mutex = &mutex_holder;
    condition = &condition_holder;

    printf ("create thread\n");
    fflush (stdout);
    
    mutex->Lock ();

    Thread thread1 (&run), thread2 (&run);
    
    printf ("sleep\n");
    fflush (stdout);    
    
    mutex->Unlock ();    

    sleep (1);
    
    printf ("wake\n");
    fflush (stdout);
    
    condition->Notify (true);
    
    thread1.Join ();
    thread2.Join ();    

    printf ("exit\n");
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
