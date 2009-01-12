#include "shared.h"


Semaphore* volatile semaphore = 0;
volatile   bool flag          = false;

int run ()
{
  printf ("in thread\n");
  
  printf ("thread: wait job\n");    
    
  flag = true;
    
  semaphore->Wait ();
    
  printf ("job done\n");
  
  return 0;
}

int main ()
{
  printf ("Results of semaphore_test:\n");
  
  try
  {
    LogFilter filter ("system.threads.*", &print_log);
    
    printf ("create semaphore\n");
    
    Semaphore semaphore_holder;
    
    semaphore = &semaphore_holder;    
    
    Thread thread (&run);        
    
    while (!flag);
            
    printf ("add job\n");

    semaphore->Post ();

    thread.Join ();
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
