#include "shared.h"

int main ()
{
  printf ("Results of get_current_thread_test:\n");

  try    
  {
    LogFilter filter ("common.threads.*", &print_log);
    
    Thread thread = Thread::GetCurrent ();

      //???
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
