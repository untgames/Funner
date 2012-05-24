#include "shared.h"

using namespace plarium::system;

void* run (void*)
{
  printf ("in another thread\n");
  
  return (void*)12;
}

int main ()
{
  printf ("Results of create_thread_test:\n");

  try    
  {
    Thread thread (&run);

    printf ("thread exit with code %d\n", thread.Join ());
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
