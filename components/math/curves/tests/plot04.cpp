#include "shared.h"

int main ()
{
  printf ("Results of plot04_test:\n");
  
  try
  {
    tcb_spline2f spline;
    
    typedef tcb_spline2f::key_type key;
    
    spline.add_key (key (1.0f, vec2f (0.0f, 1.0f), 0.0f, 1.0f, 0.0f));
    spline.add_key (key (2.0f, vec2f (1.0f, 0.0f), 0.0f, 1.0f, 0.0f));
    spline.add_key (key (3.0f, vec2f (0.0f, -1.0f), 0.0f, 1.0f, 0.0f));
    spline.add_key (key (4.0f, vec2f (-1.0f, 0.0f), 0.0f, 1.0f, 0.0f));
    spline.add_key (key (5.0f, vec2f (0.0f, 1.0f), 0.0f, 1.0f, 0.0f));
    
    dump ("closed", spline.closed ());
    
    plot_spline ("four points tcb spline2", spline, 80, 80);  
  }
  catch (std::exception& e)
  {
    printf ("exception: %s\n", e.what ());
  }

  return 0;
}
