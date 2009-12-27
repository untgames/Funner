#include "shared.h"

int main ()
{
  printf ("Results of plot02_test:\n");
  
  try
  {
    tcb_splinef spline;
    
    spline.add_key (2.0f, 1.0f);
    spline.add_key (1.0f, 0.0f);
    spline.add_key (3.0f, -1.0f);
    
    plot_spline ("three points tcb spline", spline, 80, 80);  
  }
  catch (std::exception& e)
  {
    printf ("exception: %s\n", e.what ());
  }

  return 0;
}
