#include "shared.h"

using namespace SPINE_NAMESPACE_NAME;
using namespace media::SPINE_NAMESPACE_NAME;

/*
   Constructor
*/

SkeletonDataSpineImpl::SkeletonDataSpineImpl (SpineAtlasPtr in_atlas, SkeletonClippingSpineImplPtr in_clipper, SpineSkeletonDataPtr in_skeleton_data)
  : atlas (in_atlas)
  , clipper (in_clipper)
  , skeleton_data (in_skeleton_data)
  {}


/*
   Create object instances
*/

media::spine::SkeletonImpl* SkeletonDataSpineImpl::CreateSkeleton ()
{
  SpineSkeletonPtr skeleton (new SpineHandleHolder<spSkeleton> (spSkeleton_create (skeleton_data->NativeHandle ()), spSkeleton_dispose), false);

  if (!skeleton->NativeHandle ())
    throw xtl::format_operation_exception ("media::spine::SkeletonDataSpineImpl::CreateSkeleton", "Can't create skeleton");

  return new SkeletonSpineImpl (atlas, clipper, skeleton_data, skeleton);
}

media::spine::AnimationStateDataImpl* SkeletonDataSpineImpl::CreateAnimationStateData ()
{
  SpineAnimationStateDataPtr animation_state_data (new SpineHandleHolder<spAnimationStateData> (spAnimationStateData_create (skeleton_data->NativeHandle ()), spAnimationStateData_dispose), false);

  if (!animation_state_data->NativeHandle ())
    throw xtl::format_operation_exception ("media::spine::SkeletonDataSpineImpl::CreateAnimationStateData", "Can't create animation state data");

  return new AnimationStateDataSpineImpl (atlas, skeleton_data, animation_state_data);
}

/*
   Params
*/

float SkeletonDataSpineImpl::Width ()
{
  return skeleton_data->NativeHandle ()->width;
}

float SkeletonDataSpineImpl::Height ()
{
  return skeleton_data->NativeHandle ()->height;
}

/*
   Animations
*/

unsigned int SkeletonDataSpineImpl::AnimationsCount ()
{
  return skeleton_data->NativeHandle ()->animationsCount;
}

const char* SkeletonDataSpineImpl::AnimationName (unsigned int index)
{
  return skeleton_data->NativeHandle ()->animations [index]->name;
}

float SkeletonDataSpineImpl::AnimationDuration (unsigned int index)
{
  return skeleton_data->NativeHandle ()->animations [index]->duration;
}

/*
   Skins
*/

unsigned int SkeletonDataSpineImpl::SkinsCount ()
{
  return skeleton_data->NativeHandle ()->skinsCount;
}

const char* SkeletonDataSpineImpl::SkinName (unsigned int index)
{
  return skeleton_data->NativeHandle ()->skins [index]->name;
}

const char* SkeletonDataSpineImpl::DefaultSkinName ()
{
  if (!skeleton_data->NativeHandle ()->defaultSkin)
    return 0;

  return skeleton_data->NativeHandle ()->defaultSkin->name;
}

/*
   Get native handle
*/

spSkeletonData* SkeletonDataSpineImpl::NativeHandle ()
{
  return skeleton_data->NativeHandle ();
}
