#include "shared.h"

Mutex*     mutex = 0;
Condition* condition = 0;

int run ()
{
  syslib::Lock lock (*mutex);
  
  printf ("wait started\n");  
  fflush (stdout);

  condition->Wait (*mutex);

  printf ("wait finished\n");
  fflush (stdout);

  return 0;
}

int main ()
{
  printf ("Results of condition_broadcast_test:\n");

  try
  {
    LogFilter filter ("system.threads.*", &print_log);

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

    Application::Sleep (3000);
    
    printf ("wake\n");
    fflush (stdout);
    
    condition->NotifyAll ();
    
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
