#include "shared.h"

const char* DST_FILE_NAME = "/io/stdout/test.xanim";

using namespace media::animation;

int main ()
{
  printf ("Results of xanim_saver_test:\n");
  
  try
  {
    Animation animation1, animation2;

    animation1.Rename ("animation1");

    animation1.Events ().AddEvent (0.11f, 0.21f, "0.11-0.21 event");
    animation1.Events ().AddEvent (0.51f, 0.31f, "0.51-0.31 event");

    math::tcb_spline3f      channel1_track;
    math::step_spline_mat4f channel2_track;
    math::bezier_spline2f   channel3_track;

    channel1_track.add_key (math::spline_tcb_key<math::vec3f> (0.f, 1.f, 0.1f, 0.2f, 0.3f));
    channel1_track.add_key (math::spline_tcb_key<math::vec3f> (1.f, 2.f, 0.2f, 0.3f, 0.4f));
    channel1_track.add_key (math::spline_tcb_key<math::vec3f> (2.f, 3.f, 0.5f, 0.4f, 0.3f));
    channel2_track.add_key (math::spline_step_key<math::mat4f> (0.f, 1.f));
    channel2_track.add_key (math::spline_step_key<math::mat4f> (1.f, 2.f));
    channel2_track.add_key (math::spline_step_key<math::mat4f> (2.f, 3.f));
    channel3_track.add_key (math::spline_bezier_key<math::vec2f> (0.f, 1.f, 0.1f, 0.2f));
    channel3_track.add_key (math::spline_bezier_key<math::vec2f> (1.f, 2.f, 0.2f, 0.3f));
    channel3_track.add_key (math::spline_bezier_key<math::vec2f> (2.f, 3.f, 0.9f, 0.8f));

    Channel channel1, channel2, channel3;

    channel1.SetTrack (channel1_track);
    channel1.SetParameterName ("position");
    channel2.SetTrack (channel2_track);
    channel2.SetParameterName ("color");
    channel3.SetParameterName ("some_bezier_parameter");
    channel3.SetTrack (channel3_track);

    animation1.AddChannel ("channel1_target", channel1);
    animation1.AddChannel ("channel2_target", channel2);
    animation1.AddChannel ("channel3_target", channel3);

    AnimationLibrary animation_library;

    animation_library.Attach ("animation1", animation1);
    animation_library.Attach ("animation2", animation2);

    animation_library.Save (DST_FILE_NAME);
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
