#include "shared.h"

using namespace media::animation;

int main ()
{
  printf ("Results of animation_time_test:\n");
  
  try
  {
    Animation animation;

    animation.Events ().AddEvent (0.1f, 0.0f, "animation event1");
    animation.Events ().AddEvent (0.3f, 0.0f, "animation event2");

    Channel test_channel;
    
    math::tcb_spline2f spline;
    
    spline.add_key (-1.0f, 1.0f);
    spline.add_key (0.2f, 1.0f);        
    
    test_channel.SetTrack (spline);
    
    animation.AddChannel ("target1", test_channel);

    printf ("min_time=%.3f, max_time=%.3f, min_unwrapped_time=%.3f, max_unwrapped_time=%.3f\n", animation.MinTime (), animation.MaxTime (), animation.MinUnwrappedTime (),
      animation.MaxUnwrappedTime ());
  }
  catch (std::exception& exception)
  {
    printf ("Exception: %s\n", exception.what ());
  }

  return 0;
}
