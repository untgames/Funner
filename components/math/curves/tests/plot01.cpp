#include "shared.h"

int main ()
{
  printf ("Results of plot01_test:\n");
  
  try
  {
    tcb_splinef spline;
    
    spline.add_key (2.0f, 1.0f);
    spline.add_key (1.0f, 0.0f);
    
    plot_spline ("two points tcb spline", spline, 40, 40);  
  }
  catch (std::exception& e)
  {
    printf ("exception: %s\n", e.what ());
  }

  return 0;
}
