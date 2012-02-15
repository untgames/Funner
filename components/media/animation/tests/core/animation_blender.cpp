#include "shared.h"

using namespace media::animation;

class Fn
{
  public:
    typedef float result_type;

    void operator () (float time, float& value)
    {
      value = time;
    }
};

void blender_event_handler (AnimationBlenderEvent event, const char* target_name, TargetBlender&)
{
  switch (event)
  {
    case AnimationBlenderEvent_OnTargetAdded:   printf ("added target '%s'\n", target_name); break;
    case AnimationBlenderEvent_OnTargetRemoved: printf ("removed target '%s'\n", target_name); break;
    default: break;
  }
}

int main ()
{
  printf ("Results of animation_blender_test:\n");
  
  try
  {        
    Channel channel1, channel2, channel3, channel4;
    
    channel1.SetParameterName ("param1");
    channel2.SetParameterName ("param2");
    channel3.SetParameterName ("param3");
    channel4.SetParameterName ("param4");    
    
    channel1.SetTrack (Fn ());
    channel2.SetTrack (Fn ());
    channel3.SetTrack (Fn ());
    channel4.SetTrack (Fn ());
    
    Animation animation1, animation2;

    animation1.Events ().AddEvent (0.1f, 0.2f, "animation event");

    animation1.AddChannel ("test_channel_target1",  channel1);
    animation1.AddChannel ("test_channel_target1",  channel2);    
    animation1.AddChannel ("test_channel_target2",  channel3);
    animation1.AddChannel ("test_channel_target2",  channel4);

    animation2.AddChannel ("test_channel_target3",  channel1);
    animation2.AddChannel ("test_channel_target3",  channel2);    
    
    AnimationBlender blender;
    
    blender.RegisterEventHandler (AnimationBlenderEvent_OnTargetAdded, &blender_event_handler);
    blender.RegisterEventHandler (AnimationBlenderEvent_OnTargetRemoved, &blender_event_handler);
    
    printf ("add two targets to blender\n");
        
    AnimationState state1 = blender.AddSource (animation1);
    
    printf ("add one targets to blender\n");
    
    AnimationState state2 = blender.AddSource (animation2);
    
    printf ("remove first two targets\n");
    
    state1 = AnimationState ();

    printf ("exit\n");
  }
  catch (std::exception& e)
  {
    printf ("%s\n", e.what ());
  }

  return 0;
}
