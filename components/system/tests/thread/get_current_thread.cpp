#include "shared.h"

int main ()
{
  printf ("Results of get_current_thread_test:\n");

  try
  {
    LogFilter filter ("system.threads.*", &print_log);

    Thread& thread = Thread::GetCurrent ();

    thread.GetCurrent ();
//    printf ("current thread name is '%s'\n", thread.Name ());
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
