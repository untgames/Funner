#include <cstdio>

#include <xtl/bind.h>
#include <xtl/common_exceptions.h>
#include <xtl/connection.h>
#include <xtl/exception.h>
#include <xtl/function.h>

#include <common/log.h>
#include <common/time.h>

#include <media/spine/animation_state.h>
#include <media/spine/animation_state_data.h>
#include <media/spine/skeleton.h>
#include <media/spine/skeleton_data.h>
#include <media/spine/slot.h>

using namespace media::spine;

const char*  SPINE_FILE_NAME = "data/tank/export/tank-pro.skel";
const char*  ATLAS_FILE_NAME = "data/tank/export/tank.atlas";
const size_t TEST_LOOP_SIZE  = 20000;

void print_log (const char* log, const char* message)
{
  printf ("%s: %s\n", log, message);
}

int main ()
{
  printf ("Results of spine_benchmark test:\n");

  common::LogFilter log_filter ("*", &print_log);

  try
  {
    SkeletonData skeleton_data (SPINE_FILE_NAME, ATLAS_FILE_NAME);

    Skeleton skeleton = skeleton_data.CreateSkeleton ();

    if (skeleton_data.SkinsCount () > 1)
    {
      skeleton.SetSkin (skeleton_data.SkinName (1));
    }

    AnimationStateData animation_state_data = skeleton_data.CreateAnimationStateData ();

    AnimationState animation_state = animation_state_data.CreateAnimationState ();

    for (size_t i = 0, count = skeleton_data.AnimationsCount(); i < count; i++)
      animation_state.SetAnimation (i, skeleton_data.AnimationName (i), true);

    for (;;)
    {
      size_t start_time = common::milliseconds ();

      for (size_t i = 0; i < TEST_LOOP_SIZE; i++)
      {
        animation_state.Update (0.01f);
        skeleton.Apply (animation_state);
        skeleton.UpdateWorldTransform ();
        skeleton.MeshesCount (); //build meshes
      }

      printf ("Spine performance is %.2f iterations/second\n", (float)TEST_LOOP_SIZE / (float)(common::milliseconds () - start_time) * 1000.f);
    }
  }
  catch (xtl::exception& e)
  {
    printf ("Exception while testing spine file '%s': '%s'\n", SPINE_FILE_NAME, e.what ());
  }

  return 0;
}
