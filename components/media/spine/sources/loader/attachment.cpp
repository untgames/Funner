#include "shared.h"

using namespace media::spine;
using namespace SPINE_NAMESPACE_NAME;
using namespace media::SPINE_NAMESPACE_NAME;

namespace
{

media::spine::TexcoordWrap get_texcoord_wrap (spAtlasWrap wrap)
{
  switch (wrap)
  {
    case SP_ATLAS_MIRROREDREPEAT:
      return TexcoordWrap_MirroredRepeat;
    case SP_ATLAS_CLAMPTOEDGE:
      return TexcoordWrap_ClampToEdge;
    case SP_ATLAS_REPEAT:
      return TexcoordWrap_Repeat;
    default:
      throw xtl::format_operation_exception ("media::spine::AttachmentSpineImpl::TexcoordWrap", "Unknown texcoord wrap %d", wrap);
  }
}

}

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
   Params
*/

const char* AttachmentSpineImpl::TexturePath ()
{
  spAtlasRegion* atlas_region;

  switch (attachment->type)
  {
    case SP_ATTACHMENT_REGION:
    {
      spRegionAttachment* region_attachment = (spRegionAttachment*)attachment;

      atlas_region = (spAtlasRegion*)region_attachment->rendererObject;

      break;
    }
    case SP_ATTACHMENT_MESH:
    {
      spMeshAttachment* mesh_attachment = (spMeshAttachment*)attachment;

      atlas_region = (spAtlasRegion*)mesh_attachment->rendererObject;

      break;
    }
    default:
      throw xtl::format_operation_exception ("media::spine::AttachmentSpineImpl::TexturePath", "Attachment type %d doesn't have TexturePath", attachment->type);
  }

  return ((stl::string*)atlas_region->page->rendererObject)->c_str ();
}

media::spine::TexcoordWrap AttachmentSpineImpl::TexcoordWrapU ()
{
  switch (attachment->type)
  {
    case SP_ATTACHMENT_REGION:
    {
      spRegionAttachment* region_attachment = (spRegionAttachment*)attachment;
      spAtlasRegion*      atlas_region      = (spAtlasRegion*)region_attachment->rendererObject;

      return get_texcoord_wrap (atlas_region->page->uWrap);
    }
    case SP_ATTACHMENT_MESH:
    {
      spMeshAttachment* mesh_attachment = (spMeshAttachment*)attachment;
      spAtlasRegion*    atlas_region    = (spAtlasRegion*)mesh_attachment->rendererObject;

      return get_texcoord_wrap (atlas_region->page->uWrap);
    }
    default:
      throw xtl::format_operation_exception ("media::spine::AttachmentSpineImpl::TexcoordWrapU", "Attachment type %d doesn't have TexcoordWrapU", attachment->type);
  }
}

media::spine::TexcoordWrap AttachmentSpineImpl::TexcoordWrapV ()
{
  switch (attachment->type)
  {
    case SP_ATTACHMENT_REGION:
    {
      spRegionAttachment* region_attachment = (spRegionAttachment*)attachment;
      spAtlasRegion*      atlas_region      = (spAtlasRegion*)region_attachment->rendererObject;

      return get_texcoord_wrap (atlas_region->page->vWrap);
    }
    case SP_ATTACHMENT_MESH:
    {
      spMeshAttachment* mesh_attachment = (spMeshAttachment*)attachment;
      spAtlasRegion*    atlas_region    = (spAtlasRegion*)mesh_attachment->rendererObject;

      return get_texcoord_wrap (atlas_region->page->vWrap);
    }
    default:
      throw xtl::format_operation_exception ("media::spine::AttachmentSpineImpl::TexcoordWrapV", "Attachment type %d doesn't have TexcoordWrapV", attachment->type);
  }
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

math::vec4f AttachmentSpineImpl::Color ()
{
  switch (attachment->type)
  {
    case SP_ATTACHMENT_REGION:
    {
      spRegionAttachment* region_attachment = (spRegionAttachment*)attachment;

      return math::vec4f (region_attachment->color.r, region_attachment->color.g, region_attachment->color.b, region_attachment->color.a);
    }
    case SP_ATTACHMENT_MESH:
    {
      spMeshAttachment* mesh_attachment = (spMeshAttachment*)attachment;

      return math::vec4f (mesh_attachment->color.r, mesh_attachment->color.g, mesh_attachment->color.b, mesh_attachment->color.a);
    }
    default:
      throw xtl::format_operation_exception ("media::spine::AttachmentSpineImpl::Color", "Attachment type %d doesn't have Color", attachment->type);
  }
}

unsigned int AttachmentSpineImpl::VerticesCount ()
{
  switch (attachment->type)
  {
    case SP_ATTACHMENT_REGION:
    {
      return 4;
    }
    case SP_ATTACHMENT_MESH:
    {
      spMeshAttachment* mesh_attachment = (spMeshAttachment*)attachment;

      return mesh_attachment->super.worldVerticesLength / 2;
    }
    default:
      throw xtl::format_operation_exception ("media::spine::AttachmentSpineImpl::VerticesCount", "Attachment type %d doesn't have vertices", attachment->type);
  }
}

unsigned int AttachmentSpineImpl::TrianglesCount ()
{
  switch (attachment->type)
  {
    case SP_ATTACHMENT_REGION:
    {
      return 2;
    }
    case SP_ATTACHMENT_MESH:
    {
      spMeshAttachment* mesh_attachment = (spMeshAttachment*)attachment;

      return mesh_attachment->trianglesCount / 3;  //triangles count in spine is actually vertices count
    }
    default:
      throw xtl::format_operation_exception ("media::spine::AttachmentSpineImpl::VerticesCount", "Attachment type %d doesn't have vertices", attachment->type);
  }
}

void AttachmentSpineImpl::ComputeWorldVertices (media::spine::SlotImpl* slot, float* vertices, int offset, int stride)
{
  SlotSpineImpl* slot_impl = (SlotSpineImpl*)slot;

  switch (attachment->type)
  {
    case SP_ATTACHMENT_REGION:
    {
      spRegionAttachment* region_attachment = (spRegionAttachment*)attachment;

      spRegionAttachment_computeWorldVertices (region_attachment, slot_impl->NativeHandle ()->bone, vertices, offset, stride);

      break;
    }
    case SP_ATTACHMENT_MESH:
    {
      spMeshAttachment* mesh_attachment = (spMeshAttachment*)attachment;

      spVertexAttachment_computeWorldVertices (&mesh_attachment->super, slot_impl->NativeHandle (), 0, mesh_attachment->super.worldVerticesLength, vertices, offset, stride);

      break;
    }
    default:
      throw xtl::format_operation_exception ("media::spine::AttachmentSpineImpl::ComputeWorldVertices", "Attachment type %d doesn't have vertices", attachment->type);
  }
}

const float* AttachmentSpineImpl::Texcoords ()
{
  switch (attachment->type)
  {
    case SP_ATTACHMENT_REGION:
    {
      spRegionAttachment* region_attachment = (spRegionAttachment*)attachment;

      return region_attachment->uvs;
    }
    case SP_ATTACHMENT_MESH:
    {
      spMeshAttachment* mesh_attachment = (spMeshAttachment*)attachment;

      return mesh_attachment->uvs;
    }
    default:
      throw xtl::format_operation_exception ("media::spine::AttachmentSpineImpl::Texcoords", "Attachment type %d doesn't have vertices", attachment->type);
  }
}

const uint16_t* AttachmentSpineImpl::Indices ()
{
  switch (attachment->type)
  {
    case SP_ATTACHMENT_REGION:
    {
      static uint16_t indices [] = { 0, 1, 2, 2, 3, 0 };

      return indices;
    }
    case SP_ATTACHMENT_MESH:
    {
      spMeshAttachment* mesh_attachment = (spMeshAttachment*)attachment;

      return mesh_attachment->triangles;
    }
  }

  throw xtl::format_operation_exception ("media::spine::AttachmentSpineImpl::Indices", "Attachment type %d doesn't have vertices", attachment->type);
}
