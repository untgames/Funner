#include "shared.h"


Semaphore* volatile semaphore = 0;

int run ()
{
  printf ("sem wait started\n");
    
  size_t wait_start_time = milliseconds ();

  bool result = semaphore->TryWait (100);

  size_t wait_time = milliseconds () - wait_start_time;

  bool finished_in_time = wait_time > 90 && wait_time < 500;

  printf ("sem wait finished in time '%s', result = '%s'\n", finished_in_time ? "true" : "false", result ? "true" : "false");

  if (!finished_in_time)
    printf ("wait finished in %u\n", wait_time);

  return 0;
}

int main ()
{
  printf ("Results of semaphore_timed_wait test:\n");
  
  try
  {
    LogFilter filter ("system.threads.*", &print_log);
    
    Semaphore semaphore_holder;
    
    semaphore = &semaphore_holder;    
    
    Thread thread (&run);        
    
    Application::Sleep (500);
            
    thread.Join ();
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
