#include "shared.h"

int main ()
{
  printf ("Results of store_test:\n");
  
  try
  {
    Store store ("test");
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
