#include "shared.h"

using namespace media::animation;

void print (const Animation& animation)
{
  printf ("Animation name is '%s', target name is '%s'\n", animation.Name (), animation.TargetName ());
  printf ("Channels count is '%u'\n", animation.ChannelsCount ());
  printf ("Sub-animations count is '%u'\n", animation.SubAnimationsCount ());
  printf ("Events count is %u\n", animation.Events ().Size ());
}

int main ()
{
  printf ("Results of animation_test:\n");
  
  try
  {
    Animation animation;

    print (animation);

    animation.Rename ("my_animation");
    animation.SetTargetName ("my_animation_target");
    animation.Events ().AddEvent (0.1f, 0.2f, "animation event");

    Channel test_channel;
    Animation sub_animation;

    animation.AddChannel (test_channel);
    animation.AddChannel (test_channel);
    animation.AddSubAnimation (sub_animation);
    animation.AddSubAnimation (sub_animation);

    print (animation);

    animation.RemoveChannel (0);
    animation.RemoveSubAnimation (1);

    printf ("Channels count is '%u'\n", animation.ChannelsCount ());
    printf ("Sub-animations count is '%u'\n", animation.SubAnimationsCount ());

    animation.RemoveAllChannels ();
    animation.RemoveAllSubAnimations ();

    printf ("Channels count is '%u'\n", animation.ChannelsCount ());
    printf ("Sub-animations count is '%u'\n", animation.SubAnimationsCount ());

    Animation copy_test (animation), clone_test (animation.Clone ());

    copy_test = clone_test;
    copy_test.Swap (clone_test);
    swap (copy_test, clone_test);

    Animation sub_animation2;

    sub_animation.AddSubAnimation (sub_animation2);
    sub_animation2.AddSubAnimation (sub_animation);
  }
  catch (std::exception& exception)
  {
    printf ("Exception: %s\n", exception.what ());
  }

  return 0;
}
