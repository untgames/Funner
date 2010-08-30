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
  printf ("Results of condition_signal_test:\n");

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

    Thread thread (&run);
    
    printf ("sleep\n");
    fflush (stdout);    

    Application::Sleep (3000);
    
    printf ("wake\n");
    fflush (stdout);
    
    condition->NotifyOne ();
    
    thread.Join ();

    printf ("exit\n");
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
