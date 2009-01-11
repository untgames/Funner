#include <cstdio>
#include <exception>

#include <common/lockable.h>

using namespace common;

int main ()
{
  printf ("Results of lockable_test:\n");

  try
  {
    Lockable lockable;

    printf ("lock\n");
    lockable.Lock ();
    
    printf ("unlock\n");
    lockable.Unlock (); 
    
    printf ("recursive lock\n");    
    lockable.Lock ();
    lockable.Lock ();
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
