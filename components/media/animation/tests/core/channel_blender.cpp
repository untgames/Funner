#include "shared.h"

using namespace media::animation;

class Fn1
{
  public:
    typedef float result_type;

    void operator () (float time, float& value)
    {
      value = time;
    }
};

class Fn2
{
  public:
    typedef float result_type;

    void operator () (float time, float& value)
    {
      value = -time;
    }
};

class Fn3
{
  public:
    typedef double result_type;

    void operator () (float time, double& value)
    {
      value = time;
    }
};

int main ()
{
  printf ("Results of channel_blender_test:\n");
  
  try
  {
    Channel channel1, channel2;
    
    channel1.SetTrack (Fn1 ());
    channel2.SetTrack (Fn2 ());
    
    AnimationState state1, state2;

    ChannelBlender<float> blender;
    
    blender.AddChannel (state1, channel1);
    blender.AddChannel (state2, channel2);
    
    try
    {
      Channel channel3;
      
      channel3.SetTrack (Fn3 ());
      
      blender.AddChannel (state1, channel3);
    }
    catch (std::exception& e)
    {
      printf ("all right, exception detected\n", e.what ());
    }
    
    state1.SetTime (1.0f);
    state2.SetTime (1.0f);
    
    printf ("blender(1.0f): %.2f\n", blender ());
    
    printf ("remove state\n");
    
    state1 = AnimationState ();
    
    state1.SetTime (1.0f);
    
    printf ("blender(1.0f): %.2f\n", blender ());
    
    blender.AddChannel (state1, channel1);    
    
    printf ("remove channel\n");    
    
    blender.RemoveChannels (channel2);
    
    printf ("blender(1.0f): %.2f\n", blender ());        
  }
  catch (std::exception& e)
  {
    printf ("%s\n", e.what ());
  }

  return 0;
}
