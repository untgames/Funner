#include "shared.h"

using namespace media::animation;

void on_destroy ()
{
  printf ("animation state destroyed\n");
}

int main ()
{
  printf ("Results of animation_state_test:\n");
  
  AnimationState state;
  
  state.GetTrackable ().connect_tracker (&on_destroy);
  
  printf ("default: weight is %.2f time is %.2f\n", state.Weight (), state.Time ());
  
  state.SetWeight (0.5f);
  state.SetTime (1.0f);
  
  printf ("changed: weight is %.2f time is %.2f\n", state.Weight (), state.Time ());
  
  printf ("exit\n");

  return 0;
}
