#include "shared.h"


Semaphore* volatile semaphore = 0;

int run ()
{
  printf ("sem wait started\n");
    
  size_t wait_start_time = milliseconds ();

  bool result = semaphore->TryWait (100);

  size_t wait_time = milliseconds () - wait_start_time;

//  printf ("wait time = %u\n", wait_time);

  printf ("sem wait finished in time '%s', result = '%s'\n", wait_time > 99 && wait_time < 200 ? "true" : "false", result ? "true" : "false");

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
