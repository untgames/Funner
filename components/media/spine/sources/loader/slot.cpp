#include "shared.h"

using namespace SPINE_NAMESPACE_NAME;
using namespace media::SPINE_NAMESPACE_NAME;

/*
   Constructor
*/

SlotSpineImpl::SlotSpineImpl (SpineAtlasPtr in_atlas, SpineSkeletonDataPtr in_skeleton_data, SpineSkeletonPtr in_skeleton, ::SPINE_NAMESPACE_NAME::spSlot* in_slot)
  : atlas (in_atlas)
  , skeleton_data (in_skeleton_data)
  , skeleton (in_skeleton)
  , slot (in_slot)
  {}

/*
   Name
*/

const char* SlotSpineImpl::Name ()
{
  return slot->data->name;
}

/*
   Params
*/

math::vec4f SlotSpineImpl::Color ()
{
  return math::vec4f (slot->color.r, slot->color.g, slot->color.b, slot->color.a);
}

media::spine::BlendMode SlotSpineImpl::BlendMode ()
{
  switch (slot->data->blendMode)
  {
    case SP_BLEND_MODE_NORMAL:
      return media::spine::BlendMode_Normal;
    case SP_BLEND_MODE_ADDITIVE:
      return media::spine::BlendMode_Additive;
    case SP_BLEND_MODE_MULTIPLY:
      return media::spine::BlendMode_Multiply;
    case SP_BLEND_MODE_SCREEN:
      return media::spine::BlendMode_Screen;
    default:
      throw xtl::format_operation_exception ("media::spine::SlotSpineImpl::BlendMode", "Unknown blend mode %d", slot->data->blendMode);
  }
}

bool SlotSpineImpl::HasBone ()
{
  return slot->bone;
}

media::spine::BoneImpl* SlotSpineImpl::CreateBoneImpl ()
{
  if (!HasBone ())
    throw xtl::format_operation_exception ("media::spine::SlotSpineImpl::CreateBoneImpl", "No bone");

  //create new object here instead of returning existing object from skeleton, because it's much faster than searching for a bone in existing bones array
  return new BoneSpineImpl (atlas, skeleton_data, skeleton, slot->bone);
}

/*
   Attachment
*/

bool SlotSpineImpl::HasAttachment ()
{
  return slot->attachment;
}

const char* SlotSpineImpl::AttachmentName ()
{
  if (!HasAttachment ())
    throw xtl::format_operation_exception ("media::spine::SlotSpineImpl::AttachmentName", "No attachment");

  return slot->attachment->name;
}

media::spine::AttachmentImpl* SlotSpineImpl::CreateAttachmentImpl ()
{
  if (!HasAttachment ())
    throw xtl::format_operation_exception ("media::spine::SlotSpineImpl::CreateAttachmentImpl", "No attachment");

  return new AttachmentSpineImpl (atlas, skeleton_data, skeleton, slot->attachment);
}
