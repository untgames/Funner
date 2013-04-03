#include <stdio.h>

#include <memory>

extern "C"
{

__declspec (dllexport) void sampleFn ()
{
  printf ("Sample function\n"); 
//  fflush (stdout);

  throw std::bad_alloc ();
}

}
