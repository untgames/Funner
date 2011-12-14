#include "shared.h"

int main ()
{
  printf ("Results of window1_test:\n");
  
  try
  {
    Test test (L"Window1 test");
    
    test.TestChangeWindowStyle ();

    return test.Run ();
  }
  catch (std::exception& e)
  {
    printf ("%s\n", e.what ());
  }

  return 0;
}
