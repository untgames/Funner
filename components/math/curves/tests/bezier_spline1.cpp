#include "shared.h"

int main ()
{
  printf ("Results of bezier_spline1_test:\n");
  
  try
  {
    bezier_splinef spline;
    
    spline.add_key (2.0f, 1.0f);
    spline.add_key (1.0f, 0.0f);
    
    test_spline ("two points spline", 1.0f, 2.0f, spline);
  }
  catch (std::exception& e)
  {
    printf ("exception: %s\n", e.what ());
  }

  return 0;
}
