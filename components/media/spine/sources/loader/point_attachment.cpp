#include "shared.h"

using namespace SPINE_NAMESPACE_NAME;
using namespace media::SPINE_NAMESPACE_NAME;

/*
   Constructor
*/

PointAttachmentDataSpineImpl::PointAttachmentDataSpineImpl (SpineAtlasPtr in_atlas, SpineSkeletonDataPtr in_skeleton_data, SpineSkeletonPtr in_skeleton, ::SPINE_NAMESPACE_NAME::spPointAttachment* in_attachment)
  : atlas (in_atlas)
  , skeleton_data (in_skeleton_data)
  , skeleton (in_skeleton)
  , attachment (in_attachment)
  {}

/*
   Point attachment parameters
*/

math::vec2f PointAttachmentDataSpineImpl::Position ()
{
  return math::vec2f (attachment->x, attachment->y);
}

float PointAttachmentDataSpineImpl::Rotation ()
{
  return attachment->rotation;
}

math::vec4f PointAttachmentDataSpineImpl::Color ()
{
  return math::vec4f (attachment->color.r, attachment->color.g, attachment->color.b, attachment->color.a);
}
