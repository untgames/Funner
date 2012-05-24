#include "shared.h"

using namespace plarium::system;

void* run (void*)
{
  throw std::logic_error ("Operation failed");
}

int main ()
{
  printf ("Results of thread_exception_test:\n");

  try    
  {
    Thread thread (&run);

    thread.Join ();
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
