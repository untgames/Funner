#include <cstdio>

#include <stdexcept>

#include <analytics/flurry.h>

using namespace analytics::flurry;

int main ()
{
  printf ("Results of flurry_test:\n");
  
  try
  {
    printf ("Flurry supported = %d\n", Flurry::IsSupported ());
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
