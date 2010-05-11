#include "shared.h"

using namespace media::animation;

void print (const Animation& animation)
{
  printf ("Animation name is '%s'\n", animation.Name ());
  printf ("Events count is %u\n", animation.Events ().Size ());
  printf ("Targets count is '%u'\n", animation.TargetsCount ());

  for (size_t i = 0, targets_count = animation.TargetsCount (); i < targets_count; i++)
    printf ("Target %u name is '%s', channels count is %u\n", i, animation.TargetName (i), animation.ChannelsCount (i));
}

int main ()
{
  printf ("Results of animation_test:\n");
  
  try
  {
    Animation animation;

    print (animation);

    animation.Rename ("my_animation");
    animation.Events ().AddEvent (0.1f, 0.2f, "animation event");

    Channel test_channel;

    animation.AddChannel ("test_channel_target", test_channel);
    animation.AddChannel ("test_channel_target2", test_channel);

    print (animation);

    animation.RemoveChannel (0, 0);

    printf ("Targets count is '%u'\n", animation.TargetsCount ());

    animation.RemoveAllChannels ();

    printf ("Targets count is '%u'\n", animation.TargetsCount ());

    Animation copy_test (animation), clone_test (animation.Clone ());

    copy_test = clone_test;
    copy_test.Swap (clone_test);
    swap (copy_test, clone_test);
  }
  catch (std::exception& exception)
  {
    printf ("Exception: %s\n", exception.what ());
  }

  return 0;
}
