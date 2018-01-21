#include <xtl/common_exceptions.h>

#include "bone_impl.h"
#include "skeleton_impl.h"
#include "slot_impl.h"
#include "wrappers.h"

using namespace media::spine;

/*
   Setup methods
*/

void SkeletonImpl::ResizeSlots (unsigned int size)
{
  slots.resize (size);
}

void SkeletonImpl::ResizeBones (unsigned int size)
{
  bones.resize (size);
}

/*
   Work with slots
*/

media::spine::Slot& SkeletonImpl::Slot (unsigned int index)
{
  if (index >= slots.size ())
    throw xtl::make_range_exception ("media::spine::SkeletonImpl::Slot", "index", index, 0u, slots.size ());

  if (!slots [index])
    slots [index] = SlotPtr (Wrappers::WrapToNew<media::spine::Slot, media::spine::SlotImpl> (CreateSlotImpl (index)));

  return *slots [index];
}

Bone& SkeletonImpl::RootBone ()
{
  int index = RootBoneIndex ();

  //should never occur, check this just in case
  if (index < 0)
    throw xtl::format_operation_exception ("media::spine::SkeletonImpl::RootBone", "Can't find root bone");

  return Bone (index);
}

Bone& SkeletonImpl::Bone (unsigned int index)
{
  if (index >= bones.size ())
    throw xtl::make_range_exception ("media::spine::SkeletonImpl::Bone", "index", index, 0u, bones.size ());

  if (!bones [index])
    bones [index] = BonePtr (Wrappers::WrapToNew<media::spine::Bone, media::spine::BoneImpl> (CreateBoneImpl (index)));

  return *bones [index];
}
