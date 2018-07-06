#include "shared.h"

using namespace SPINE_NAMESPACE_NAME;
using namespace media::SPINE_NAMESPACE_NAME;

/*
   Constructor
*/

BoneSpineImpl::BoneSpineImpl (SpineAtlasPtr in_atlas, SpineSkeletonDataPtr in_skeleton_data, SpineSkeletonPtr in_skeleton, ::SPINE_NAMESPACE_NAME::spBone* in_bone)
  : atlas (in_atlas)
  , skeleton_data (in_skeleton_data)
  , skeleton (in_skeleton)
  , bone (in_bone)
  {}

/*
   Name
*/

const char* BoneSpineImpl::Name ()
{
  return bone->data->name;
}

/*
   Transform
*/

void BoneSpineImpl::SetPosition (const math::vec2f& position)
{
  bone->x = position.x;
  bone->y = position.y;
}

void BoneSpineImpl::SetPosition (float position_x, float position_y)
{
  bone->x = position_x;
  bone->y = position_y;
}

math::vec2f BoneSpineImpl::Position ()
{
  return math::vec2f (bone->x, bone->y);
}

void BoneSpineImpl::SetRotation (float rotation)
{
  bone->rotation = rotation;
}

float BoneSpineImpl::Rotation ()
{
  return bone->rotation;
}

void BoneSpineImpl::SetScale (const math::vec2f& scale)
{
  bone->scaleX = scale.x;
  bone->scaleY = scale.y;
}

void BoneSpineImpl::SetScale (float scale_x, float scale_y)
{
  bone->scaleX = scale_x;
  bone->scaleY = scale_y;
}

math::vec2f BoneSpineImpl::Scale ()
{
  return math::vec2f (bone->scaleX, bone->scaleY);
}

void BoneSpineImpl::SetShear (const math::vec2f& shear)
{
  bone->shearX = shear.x;
  bone->shearY = shear.y;
}

void BoneSpineImpl::SetShear (float shear_x, float shear_y)
{
  bone->shearX = shear_x;
  bone->shearY = shear_y;
}

math::vec2f BoneSpineImpl::Shear ()
{
  return math::vec2f (bone->shearX, bone->shearY);
}

math::vec2f BoneSpineImpl::WorldPosition ()
{
  return math::vec2f (bone->worldX, bone->worldY);
}

math::vec2f BoneSpineImpl::WorldRotation ()
{
  return math::vec2f (spBone_getWorldRotationX (bone), spBone_getWorldRotationY (bone));
}

math::vec2f BoneSpineImpl::WorldScale ()
{
  return math::vec2f (spBone_getWorldScaleX (bone), spBone_getWorldScaleY (bone));
}

/*
   Helpers for converting between coordinate spaces
*/

math::vec2f BoneSpineImpl::WorldToLocal (const math::vec2f& position)
{
  float x, y;

  spBone_worldToLocal (bone, position.x, position.y, &x, &y);

  return math::vec2f (x, y);
}

math::vec2f BoneSpineImpl::WorldToLocal (float position_x, float position_y)
{
  float x, y;

  spBone_worldToLocal (bone, position_x, position_y, &x, &y);

  return math::vec2f (x, y);
}

float BoneSpineImpl::WorldToLocal (float rotation)
{
  return spBone_worldToLocalRotation (bone, rotation);
}

math::vec2f BoneSpineImpl::LocalToWorld (const math::vec2f& position)
{
  float x, y;

  spBone_localToWorld (bone, position.x, position.y, &x, &y);

  return math::vec2f (x, y);
}

math::vec2f BoneSpineImpl::LocalToWorld (float position_x, float position_y)
{
  float x, y;

  spBone_localToWorld (bone, position_x, position_y, &x, &y);

  return math::vec2f (x, y);
}

float BoneSpineImpl::LocalToWorld (float rotation)
{
  return spBone_localToWorldRotation (bone, rotation);
}

/*
   Linked bones, returned object is owned by callee.
*/

bool BoneSpineImpl::HasParent ()
{
  return bone->parent != 0;
}

media::spine::BoneImpl* BoneSpineImpl::CreateParentImpl ()
{
  return new BoneSpineImpl (atlas, skeleton_data, skeleton, bone->parent);
}

unsigned int BoneSpineImpl::ChildrenCount ()
{
  return bone->childrenCount;
}

media::spine::BoneImpl* BoneSpineImpl::CreateChildImpl (unsigned int index)
{
  return new BoneSpineImpl (atlas, skeleton_data, skeleton, bone->children [index]);
}
