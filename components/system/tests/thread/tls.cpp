#include "shared.h"

const int THREADS_COUNT = 10;

ThreadLocalStorage* tls = 0;
bool                touch_set [THREADS_COUNT];

void cleanup (void* data)
{
  int index = (int)data;
  
  if (index < 0 || index >= THREADS_COUNT)
    return;
    
  touch_set [index] = true;
}

int run ()
{
  if (!tls)
    return 0;    
    
  static int count = 0;

  tls->SetValue ((void*)xtl::atomic_increment (count));

  return 0;
}

int main ()
{
  printf ("Results of tls_test:\n");
  
  try
  {
    LogFilter filter ("common.threads.*", &print_log);
    
    printf ("create tls...\n");

    ThreadLocalStorage tls_holder (&cleanup);

    tls = &tls_holder;
    
    printf ("create threads...\n");    
    
    xtl::shared_ptr<Thread> threads [THREADS_COUNT];

    for (int i=0; i<THREADS_COUNT; i++)
    {
      threads [i] = xtl::shared_ptr<Thread> (new Thread (&run));
    }
    
    printf ("join threads...\n");

    for (int i=0; i<THREADS_COUNT; i++)
      threads [i]->Join ();
      
    printf ("check cleanup...\n");

    for (int i=0; i<THREADS_COUNT; i++)
      if (!touch_set [i])
        printf ("fail at index %d\n");

    printf ("exit\n");        
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
