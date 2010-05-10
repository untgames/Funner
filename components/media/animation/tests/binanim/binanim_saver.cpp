#include "shared.h"

const char* RESULTS_DIR   = "results";
const char* DST_FILE_NAME = "results/test.binanim";

using namespace media::animation;

void dump_hash (const char* file_name)
{
  common::FileHash file_hash;

  common::FileSystem::GetFileHash (file_name,file_hash);

  printf ("File '%s' hash: {", file_name);

  for (size_t i=0;i<15;i++)
    printf ("%02x,",file_hash.md5 [i]);

  printf ("%02x}\n",file_hash.md5 [15]);
}

int main ()
{
  printf ("Results of binanim_saver_test:\n");
  
  try
  {
    if (!common::FileSystem::IsDir (RESULTS_DIR))
      common::FileSystem::Mkdir (RESULTS_DIR);

    Animation animation1, animation2, sub_animation;

    animation1.Rename ("animation1");
    animation1.SetTargetName ("animation1 target");
    sub_animation.Rename ("sub_animation");
    sub_animation.SetTargetName ("sub_animation target");

    animation1.Events ().AddEvent (0.11f, 0.21f, "0.11-0.21 event");
    animation1.Events ().AddEvent (0.51f, 0.31f, "0.51-0.31 event");
    sub_animation.Events ().AddEvent (0.1f, 0.2f, "0.1-0.2 event");
    sub_animation.Events ().AddEvent (0.5f, 0.3f, "0.5-0.3 event");

    math::tcb_spline3f      channel1_track;
    math::step_spline_mat4f channel2_track;
    math::bezier_spline2f   sub_animation_channel_track;

    channel1_track.add_key (math::spline_tcb_key<math::vec3f> (0.f, 1.f, 0.1f, 0.2f, 0.3f));
    channel1_track.add_key (math::spline_tcb_key<math::vec3f> (1.f, 2.f, 0.2f, 0.3f, 0.4f));
    channel1_track.add_key (math::spline_tcb_key<math::vec3f> (2.f, 3.f, 0.5f, 0.4f, 0.3f));
    channel2_track.add_key (math::spline_step_key<math::mat4f> (0.f, 1.f));
    channel2_track.add_key (math::spline_step_key<math::mat4f> (1.f, 2.f));
    channel2_track.add_key (math::spline_step_key<math::mat4f> (2.f, 3.f));
    sub_animation_channel_track.add_key (math::spline_bezier_key<math::vec2f> (0.f, 1.f, 0.1f, 0.2f));
    sub_animation_channel_track.add_key (math::spline_bezier_key<math::vec2f> (1.f, 2.f, 0.2f, 0.3f));
    sub_animation_channel_track.add_key (math::spline_bezier_key<math::vec2f> (2.f, 3.f, 0.9f, 0.8f));

    Channel channel1, channel2, sub_animation_channel;

    channel1.SetTrack (channel1_track);
    channel1.SetParameterName ("position");
    channel2.SetTrack (channel2_track);
    channel2.SetParameterName ("color");
    sub_animation_channel.SetParameterName ("some_bezier_parameter");
    sub_animation_channel.SetTrack (sub_animation_channel_track);

    animation1.AddChannel (channel1);
    animation1.AddChannel (channel2);
    sub_animation.AddChannel (sub_animation_channel);

    animation1.AddSubAnimation (sub_animation);

    AnimationLibrary animation_library;

    animation_library.Attach ("animation1", animation1);
    animation_library.Attach ("animation2", animation2);

    animation_library.Save (DST_FILE_NAME);

    dump_hash (DST_FILE_NAME);
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
