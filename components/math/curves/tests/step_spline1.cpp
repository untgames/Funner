#include "shared.h"

int main ()
{
  printf ("Results of step_spline1_test:\n");
  
  try
  {
    step_spline3f spline;

    spline.add_key (1.f, 1.f);
    spline.add_key (2.f, 2.f);

    test_spline ("two points vec3f spline", 0.9f, 2.1f, spline);
  }
  catch (std::exception& e)
  {
    printf ("exception: %s\n", e.what ());
  }

  try
  {
    step_spline4f spline;

    spline.add_key (1.f, 1.f);
    spline.add_key (2.f, 2.f);

    test_spline ("two points vec4f spline", 0.9f, 2.1f, spline);
  }
  catch (std::exception& e)
  {
    printf ("exception: %s\n", e.what ());
  }

  try
  {
    step_spline_mat4f spline;
    
    spline.add_key (1.f, 1.f);
    spline.add_key (2.f, 2.f);
    
    test_spline ("two points mat4f spline", 0.9f, 2.1f, spline);
  }
  catch (std::exception& e)
  {
    printf ("exception: %s\n", e.what ());
  }

  return 0;
}
