#include "shared.h"

int run ()
{
  throw xtl::format_operation_exception ("::run", "Operation failed");
}

int main ()
{
  printf ("Results of thread_exception_test:\n");

  try    
  {
    LogFilter filter ("common.threads.*", &print_log);
    
    Thread thread (&run);

    thread.Join ();
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
