#include <stdio.h>

#include <memory>

extern "C"
{

__declspec (dllexport) void sampleFn ()
{
  printf ("Sample function\n"); 
//  fflush (stdout);

  try
  {
    throw std::bad_alloc ();
  }
  catch (std::exception& e)
  {
    printf ("exception was thrown: %s\n", e.what ());
  }
  catch (...)
  {
    printf ("unknown exception was thrown\n");
  }
}

}
