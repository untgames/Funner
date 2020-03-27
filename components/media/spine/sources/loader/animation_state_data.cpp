#include "shared.h"

using namespace SPINE_NAMESPACE_NAME;
using namespace media::SPINE_NAMESPACE_NAME;

/*
   Constructor
*/

AnimationStateDataSpineImpl::AnimationStateDataSpineImpl (SpineAtlasPtr in_atlas, SpineSkeletonDataPtr in_skeleton_data, SpineAnimationStateDataPtr in_animation_state_data)
  : atlas (in_atlas)
  , skeleton_data (in_skeleton_data)
  , animation_state_data (in_animation_state_data)
  {}

/*
   Create object instance
*/

media::spine::AnimationStateImpl* AnimationStateDataSpineImpl::CreateAnimationState ()
{
  SpineAnimationStatePtr animation_state (new SpineHandleHolder<spAnimationState> (spAnimationState_create (animation_state_data->NativeHandle ()), spAnimationState_dispose), false);

  if (!animation_state->NativeHandle ())
    throw xtl::format_operation_exception ("media::spine::AnimationStateDataSpineImpl::CreateAnimationState", "Can't create animation state");

  return new AnimationStateSpineImpl (atlas, skeleton_data, animation_state_data, animation_state);
}

/*
   Animation mixing
*/

float AnimationStateDataSpineImpl::DefaultMix ()
{
  return animation_state_data->NativeHandle ()->defaultMix;
}

void AnimationStateDataSpineImpl::SetDefaultMix (float default_mix)
{
  animation_state_data->NativeHandle ()->defaultMix = default_mix;
}

float AnimationStateDataSpineImpl::GetMix (const char* animation_from, const char* animation_to)
{
  spAnimation* from = spSkeletonData_findAnimation (skeleton_data->NativeHandle (), animation_from);

  if (!from)
    return DefaultMix ();

  spAnimation* to = spSkeletonData_findAnimation (skeleton_data->NativeHandle (), animation_to);

  if (!to)
    return DefaultMix ();

  return spAnimationStateData_getMix(animation_state_data->NativeHandle (), from, to);
}

void AnimationStateDataSpineImpl::SetMix (const char* animation_from, const char* animation_to, float mix)
{
  spAnimationStateData_setMixByName (animation_state_data->NativeHandle (), animation_from, animation_to, mix);
}
