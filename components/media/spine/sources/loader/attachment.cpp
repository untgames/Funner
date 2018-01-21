#include "shared.h"

using namespace media::spine;
using namespace SPINE_NAMESPACE_NAME;
using namespace media::SPINE_NAMESPACE_NAME;

/*
   Constructor
*/

AttachmentSpineImpl::AttachmentSpineImpl (SpineAtlasPtr in_atlas, SpineSkeletonDataPtr in_skeleton_data, SpineSkeletonPtr in_skeleton, ::SPINE_NAMESPACE_NAME::spAttachment* in_attachment)
  : atlas (in_atlas)
  , skeleton_data (in_skeleton_data)
  , skeleton (in_skeleton)
  , attachment (in_attachment)
  {}

/*
   Name
*/

const char* AttachmentSpineImpl::Name ()
{
  return attachment->name;
}

/*
   Get Type and type-specific data.
*/

AttachmentType AttachmentSpineImpl::Type ()
{
  switch (attachment->type)
  {
    case SP_ATTACHMENT_REGION:       return AttachmentType_Region;
    case SP_ATTACHMENT_BOUNDING_BOX: return AttachmentType_BoundingBox;
    case SP_ATTACHMENT_MESH:         return AttachmentType_Mesh;
    case SP_ATTACHMENT_LINKED_MESH:  return AttachmentType_LinkedMesh;
    case SP_ATTACHMENT_PATH:         return AttachmentType_Path;
    case SP_ATTACHMENT_POINT:        return AttachmentType_Point;
    case SP_ATTACHMENT_CLIPPING:     return AttachmentType_Clipping;
    default:                         return AttachmentType_Unknown;
  }
}

media::spine::PointAttachmentDataImpl* AttachmentSpineImpl::CreatePointAttachmentDataImpl ()
{
  if (attachment->type != SP_ATTACHMENT_POINT)
    throw xtl::format_operation_exception ("media::spine::AttachmentSpineImpl::CreatePointAttachmentDataImpl", "Wrong data type requested");

  return new PointAttachmentDataSpineImpl (atlas, skeleton_data, skeleton, (spPointAttachment*)attachment);
}
