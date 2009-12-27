#include "shared.h"

int main ()
{
  printf ("Results of tcb_spline1_test:\n");
  
  try
  {
    tcb_splinef spline;
    
    test_spline ("empty spline", -2.0f, 3.0f, spline);
  }
  catch (std::exception& e)
  {
    printf ("exception: %s\n", e.what ());
  }
  
  try
  {
    tcb_splinef spline;
    
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
