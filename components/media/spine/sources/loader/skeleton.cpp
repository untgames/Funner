#include "shared.h"

using namespace SPINE_NAMESPACE_NAME;
using namespace media::SPINE_NAMESPACE_NAME;

namespace
{

//Constants
const char* LOG_NAME = "media.spine.SkeletonSpineImpl"; //log stream name

}

/*
   Constructor
*/

SkeletonSpineImpl::SkeletonSpineImpl (SpineAtlasPtr in_atlas, SkeletonClippingSpineImplPtr in_clipper, SpineSkeletonDataPtr in_skeleton_data, SpineSkeletonPtr in_skeleton)
  : atlas (in_atlas)
  , clipper (in_clipper)
  , skeleton_data (in_skeleton_data)
  , skeleton (in_skeleton)
{
  ResizeSlots (SlotsCount ());
  ResizeBones (BonesCount ());

  spSkeleton* native_handle = skeleton->NativeHandle ();

  for (unsigned int i = 0; i < native_handle->slotsCount; ++i)
    slots_index_map.insert_pair (native_handle->slots [i], i);
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

  //do not need to invalidate mesh, because geometry will be modified only after updating world transform
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

media::spine::SlotImpl* SkeletonSpineImpl::CreateSlotForDrawOrder (unsigned int index)
{
  return new SlotSpineImpl (atlas, skeleton_data, skeleton, skeleton->NativeHandle ()->drawOrder [index]);
}

media::spine::SlotImpl* SkeletonSpineImpl::CreateSlotImpl (unsigned int index)
{
  return new SlotSpineImpl (atlas, skeleton_data, skeleton, skeleton->NativeHandle ()->slots [index]);
}

int SkeletonSpineImpl::FindSlotIndex (const char* name)
{
  return spSkeleton_findSlotIndex (skeleton->NativeHandle (), name);
}

int SkeletonSpineImpl::SlotForDrawOrder (unsigned int index)
{
  spSlot* slot = skeleton->NativeHandle ()->drawOrder [index];

  SlotToIndexMap::iterator iter = slots_index_map.find (slot);

  if (iter == slots_index_map.end ())
  {
    common::Log (LOG_NAME).Printf ("Couldn't find spine slot for draw index %u", index);
    return -1;
  }

  return iter->second;
}

/*
   Rendering
*/


/*
   Change skin (use 0 to set default skin)
*/

const char* SkeletonSpineImpl::Skin ()
{
  return skeleton->NativeHandle ()->skin ? skeleton->NativeHandle ()->skin->name : 0;
}

bool SkeletonSpineImpl::SetSkin (const char* skin_name)
{
  InvalidateMeshes ();

  return spSkeleton_setSkinByName (skeleton->NativeHandle (), skin_name);
}

bool SkeletonSpineImpl::SetAttachment (const char* slot_name, const char* attachment_name)
{
  InvalidateMeshes ();

  return spSkeleton_setAttachment (skeleton->NativeHandle (), slot_name, attachment_name);
}

/*
   Update transform after animation/bones manipulation
*/

void SkeletonSpineImpl::UpdateWorldTransform ()
{
  spSkeleton_updateWorldTransform (skeleton->NativeHandle ());

  InvalidateMeshes ();
}

/*
   Helper methods
*/

media::spine::MaterialImpl* SkeletonSpineImpl::CreateMaterialImpl (const char* material_name, const char* texture_path, media::spine::BlendMode blend_mode, media::spine::TexcoordWrap texcoord_wrap_u, media::spine::TexcoordWrap texcoord_wrap_v)
{
  return new MaterialSpineImpl (material_name, texture_path, blend_mode, texcoord_wrap_u, texcoord_wrap_v);
}

media::spine::SkeletonClippingImpl* SkeletonSpineImpl::Clipper ()
{
  return clipper.get ();
}
