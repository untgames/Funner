#include "shared.h"

int run ()
{
  printf ("in another thread\n");
  
  return 12;
}

int main ()
{
  printf ("Results of create_thread_test:\n");

  try    
  {
    LogFilter filter ("common.threads.*", &print_log);
    
    Thread thread (&run);

    printf ("thread exit with code %d\n", thread.Join ());
//    printf ("join again, code %d\n", thread.Join ());    
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
