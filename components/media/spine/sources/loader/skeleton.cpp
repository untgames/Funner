#include "shared.h"

using namespace SPINE_NAMESPACE_NAME;
using namespace media::SPINE_NAMESPACE_NAME;

/*
   Constructor
*/

SkeletonSpineImpl::SkeletonSpineImpl (SpineAtlasPtr in_atlas, SpineSkeletonDataPtr in_skeleton_data, SpineSkeletonPtr in_skeleton)
  : atlas (in_atlas)
  , skeleton_data (in_skeleton_data)
  , skeleton (in_skeleton)
{
  ResizeSlots (SlotsCount ());
  ResizeBones (BonesCount ());
}

/*
   Animating
*/

void SkeletonSpineImpl::SetToSetupPose ()
{
  spSkeleton_setToSetupPose (skeleton->NativeHandle ());
}

void SkeletonSpineImpl::ApplyAnimationState (media::spine::AnimationStateImpl* animation)
{
  //we do not need return value, it indicate if animation state was applied or not (because of track not started yet for example)
  spAnimationState_apply (((AnimationStateSpineImpl*)animation)->NativeHandle (), skeleton->NativeHandle ());
}

/*
   Work with bones
*/

unsigned int SkeletonSpineImpl::BonesCount ()
{
  return skeleton->NativeHandle ()->bonesCount;
}

media::spine::BoneImpl* SkeletonSpineImpl::CreateBoneImpl (unsigned int index)
{
  return new BoneSpineImpl (atlas, skeleton_data, skeleton, skeleton->NativeHandle ()->bones [index]);
}

int SkeletonSpineImpl::RootBoneIndex ()
{
  spSkeleton* spine_skeleton = skeleton->NativeHandle ();

  for (int i = 0; i < spine_skeleton->bonesCount; i++)
  {
    if (spine_skeleton->root == spine_skeleton->bones[i])
      return i;
  }

  return -1;
}

int SkeletonSpineImpl::FindBoneIndex (const char* name)
{
  return spSkeleton_findBoneIndex (skeleton->NativeHandle (), name);
}

/*
   Work with slots
*/

unsigned int SkeletonSpineImpl::SlotsCount ()
{
  return skeleton->NativeHandle ()->slotsCount;
}

media::spine::SlotImpl* SkeletonSpineImpl::CreateSlotImpl (unsigned int index)
{
  return new SlotSpineImpl (atlas, skeleton_data, skeleton, skeleton->NativeHandle ()->slots [index]);
}

int SkeletonSpineImpl::FindSlotIndex (const char* name)
{
  return spSkeleton_findSlotIndex (skeleton->NativeHandle (), name);
}

/*
   Rendering
*/

unsigned int SkeletonSpineImpl::MeshesCount ()
{
  //TODO
  return 0;
}

media::geometry::Mesh SkeletonSpineImpl::Mesh (unsigned int mesh_index)
{
  //TODO
  throw xtl::make_not_implemented_exception ("");
}

media::spine::MaterialImpl* SkeletonSpineImpl::Material (const char* name)
{
  //TODO
  return 0;
}

/*
   Change skin (use 0 to set default skin)
*/

const char* SkeletonSpineImpl::Skin ()
{
  return skeleton->NativeHandle ()->skin ? skeleton->NativeHandle ()->skin->name : 0;
}

bool SkeletonSpineImpl::SetSkin (const char* skin_name)
{
  return spSkeleton_setSkinByName (skeleton->NativeHandle (), skin_name);
}

bool SkeletonSpineImpl::SetAttachment (const char* slot_name, const char* attachment_name)
{
  return spSkeleton_setAttachment (skeleton->NativeHandle (), slot_name, attachment_name);
}

/*
   Update transform after animation/bones manipulation
*/

void SkeletonSpineImpl::UpdateWorldTransform ()
{
  spSkeleton_updateWorldTransform (skeleton->NativeHandle ());
}
