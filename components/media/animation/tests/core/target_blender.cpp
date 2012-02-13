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

template <class T> class Fn3
{
  public:
    typedef T result_type;

    void operator () (float time, T& value)
    {
      value = time;
    }
};

class Fn4
{
  public:
    typedef float result_type;

    void operator () (float, float&)
    {
      throw std::bad_alloc ();
    }
};

void dump_properties (const common::PropertyMap& properties)
{
  printf ("map has %u properties:\n", properties.Size ());
    
  for (size_t i=0, count=properties.Size (); i<count; i++)
  {  
    stl::string value;
    
    properties.GetProperty (i, value);
    
    printf (" #%u: name='%s', type=%s, value='%s'\n", i, properties.PropertyName (i), get_name (properties.PropertyType (i)), value.c_str ());
  }
}

int main ()
{
  printf ("Results of target_blender_test:\n");
  
  try
  {
    PropertyNameMap name_map;
    
    name_map.Set ("param", "param1");
    
    Channel channel1, channel2, channel3, channel4;
    
    channel1.SetParameterName ("param");
    channel2.SetParameterName ("param1");
    channel3.SetParameterName ("param2");
    channel4.SetParameterName ("param3");    
    
    channel1.SetTrack (Fn1 ());
    channel2.SetTrack (Fn2 ());
    channel3.SetTrack (Fn1 ());
    channel4.SetTrack (Fn2 ());    
    
    AnimationState state;

    TargetBlender blender;

    blender.AddSource (state, channel1, name_map);
    blender.AddSource (state, channel2);
    blender.AddSource (state, channel3);    
    blender.AddSource (state, channel4);

    blender.Update ();
    
    printf ("default state:\n");

    dump_properties (blender.Properties ());

    state.SetTime (1.0f);

    printf ("changed state:\n");

    blender.Update ();

    dump_properties (blender.Properties ());
    
    printf ("remove by channel:\n");
    
    blender.RemoveSource (channel3);
    
    blender.Update ();

    dump_properties (blender.Properties ());
    
    printf ("remove by property name:\n");    
    
    blender.RemoveSource ("param3");
    
    blender.Update ();

    dump_properties (blender.Properties ());    
    
    printf ("remove by state:\n");
    
    state = AnimationState ();
    
    blender.Update ();

    dump_properties (blender.Properties ());
    
    printf ("complex properties:\n");
    
    Channel vec_channel, mat_channel, quat_channel;
    
    state.SetTime (3.0f);    
    
    vec_channel.SetParameterName ("vec");
    mat_channel.SetParameterName ("matrix");
    quat_channel.SetParameterName ("quat");
    
    vec_channel.SetTrack (Fn3<math::vec3f> ());
    mat_channel.SetTrack (Fn3<math::mat3f> ());
    quat_channel.SetTrack (Fn3<math::quatf> ());    
    
    blender.AddSource (state, vec_channel);
    blender.AddSource (state, mat_channel);
    blender.AddSource (state, quat_channel);

    blender.Update ();

    dump_properties (blender.Properties ());
    
    printf ("exceptions handling:\n");
    
    Channel error_channel;
    
    error_channel.SetParameterName ("float");
    error_channel.SetTrack (Fn4 ());
    
    blender.AddSource (state, error_channel);
    
    blender.Update ();
  }
  catch (std::exception& e)
  {
    printf ("%s\n", e.what ());
  }

  return 0;
}
