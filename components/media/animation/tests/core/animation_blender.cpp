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

void print_event (float time, const char* event)
{
  printf ("    event: time=%.2f value='%s'\n", time, event);
}

void dump_targets (AnimationBlender& blender)
{
  printf ("targets_count=%u sources_count=%u sources_channels_count=%u:\n", blender.TargetsCount (), blender.SourcesCount (), blender.SourceChannelsCount ());

  for (AnimationBlender::TargetIterator iter=blender.CreateTargetIterator (); iter; ++iter)
  {
    TargetBlender& target_blender = *iter;
    const char*    target_name    = blender.TargetId (iter);
    
    target_blender.Update ();
    
    printf ("  target '%s' (sources_count=%u):\n", target_name, target_blender.SourcesCount ());
    
    common::PropertyMap properties = target_blender.Properties ();
    
    for (size_t i=0, count=properties.Size (); i<count; i++)
      printf ("    #%u: name='%s', type=%s\n", i, properties.PropertyName (i), get_name (properties.PropertyType (i)));
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
    channel3.SetParameterName ("param1");
    channel4.SetParameterName ("param3");    
    
    channel1.SetTrack (Fn ());
    channel2.SetTrack (Fn ());
    channel3.SetTrack (Fn ());
    channel4.SetTrack (Fn ());
    
    Animation animation1, animation2;

    animation1.Events ().AddEvent (0.2f, 0.2f, "animation event2");
    animation2.Events ().AddEvent (0.1f, 0.1f, "animation event1");

    animation1.AddChannel ("test_channel_target1",  channel1);
    animation1.AddChannel ("test_channel_target1",  channel2);    
    animation1.AddChannel ("test_channel_target2",  channel3);
    animation1.AddChannel ("test_channel_target2",  channel4);

    animation2.AddChannel ("test_channel_target3",  channel1);
    animation2.AddChannel ("test_channel_target3",  channel2);    
    
    AnimationBlender blender;
    
    blender.RegisterEventHandler (AnimationBlenderEvent_OnTargetAdded, &blender_event_handler);
    blender.RegisterEventHandler (AnimationBlenderEvent_OnTargetRemoved, &blender_event_handler);    
    
    printf ("-- add two targets to blender\n");
        
    AnimationState state1 = blender.AddSource (animation1);
    
    dump_targets (blender);
    
    printf ("-- add one targets to blender\n");
    
    AnimationState state2 = blender.AddSource (animation2);
    
    printf ("events:\n");
    
    state1.SetTime (1.0f);
    state2.SetTime (1.0f);
    
    blender.Update (&print_event);
    
    dump_targets (blender);
    
    printf ("-- remove first two targets\n");    
    
    state1 = AnimationState ();
    
    dump_targets (blender);

    printf ("-- exit\n");
  }
  catch (std::exception& e)
  {
    printf ("%s\n", e.what ());
  }

  return 0;
}
