#include "shared.h"

void test (const any_reference& ref)
{
  try
  {
    const char* s = ref.cast<const char*> ();
  
    printf ("cast to string ok: '%s'\n", s);
  }
  catch (std::exception& exception)
  {
    printf ("fail: %s\n", exception.what ());
  }
}

int main ()
{
  printf ("Results of any_reference_lightweight_test:\n");
  
  int i = 1;
  const char* s = "Hello world";
  
  test (i);
  test (s);  

  return 0;
}
