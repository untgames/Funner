#include "shared.h"

const char* SPINE_FILE_NAME  = "data/spine_3.6/vine/export/vine-pro.json";
const char* SPINE_ATLAS_NAME = "data/spine_3.6/vine/export/vine.atlas";

int main ()
{
  printf ("Results of spine_skeleton_test:\n");

  try
  {
    media::spine::SkeletonData skeleton_data (SPINE_FILE_NAME, SPINE_ATLAS_NAME);
    media::spine::Skeleton skeleton = skeleton_data.CreateSkeleton ();

    SpineSkeleton::Pointer model (SpineSkeleton::Create (skeleton));
  
    printf ("Is visual structure dirty: %d\n", model->IsVisualStructureDirty ());

    //check code compilable

    model->SetInfiniteBounds ();
    model->SetBoundBox (aaboxf (-10.0f, 10.0f));
  }
  catch (xtl::exception& e)
  {
    printf ("Exception caught: '%s'\n", e.what ());
  }

  return 0;
}
