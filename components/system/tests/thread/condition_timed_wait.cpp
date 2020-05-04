#include "shared.h"

Mutex*     mutex = 0;
Condition* condition = 0;

int run ()
{
  syslib::Lock lock (*mutex);
  
  printf ("wait started\n");  
  fflush (stdout);

  size_t wait_start_time = milliseconds ();

  bool result = condition->TryWait (*mutex, (size_t)100);

  size_t wait_time = milliseconds () - wait_start_time;

//  printf ("wait time = %u\n", wait_time);

  bool finished_in_time = wait_time > 90 && wait_time < 500;

  printf ("wait finished in time '%s', result = '%s'\n", finished_in_time ? "true" : "false", result ? "true" : "false");
  fflush (stdout);

  if (!finished_in_time)
  {
    printf ("wait finished in %u\n", wait_time);
    fflush (stdout);
  }

  return 0;
}

int main ()
{
  printf ("Results of condition_timed_wait_test:\n");

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

    Thread thread (&run);    
    
    printf ("sleep\n");
    fflush (stdout);    

    mutex->Unlock ();    
    
    Application::Sleep (500);
    
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
