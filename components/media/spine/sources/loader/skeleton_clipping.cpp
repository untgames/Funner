#include "shared.h"

using namespace SPINE_NAMESPACE_NAME;
using namespace media::SPINE_NAMESPACE_NAME;

/*
   Constructor
*/

SkeletonClippingSpineImpl::SkeletonClippingSpineImpl (SpineSkeletonClippingPtr in_clipper)
  : clipper (in_clipper)
  {}

/*
   Clipping
*/

bool SkeletonClippingSpineImpl::IsClipping ()
{
  return spSkeletonClipping_isClipping (clipper->NativeHandle ()) != 0;
}

void SkeletonClippingSpineImpl::ClipStart (media::spine::SlotImpl* slot, media::spine::AttachmentImpl* clip_attachment)
{
  static const char* METHOD_NAME = "media::spine::SkeletonClippingSpineImpl::ClipStart";

  if (!slot)
    throw xtl::make_null_argument_exception (METHOD_NAME, "slot");

  if (!clip_attachment)
    throw xtl::make_null_argument_exception (METHOD_NAME, "clip_attachment");

  if (clip_attachment->Type () != media::spine::AttachmentType_Clipping)
    throw xtl::make_argument_exception (METHOD_NAME, "clip_attachment", "Wrong attachment type");

  spSkeletonClipping_clipStart (clipper->NativeHandle (), ((SlotSpineImpl*)slot)->NativeHandle (), (spClippingAttachment*)((AttachmentSpineImpl*)clip_attachment)->NativeHandle ());
}

void SkeletonClippingSpineImpl::ClipEnd (media::spine::SlotImpl* slot)
{
  spSkeletonClipping_clipEnd (clipper->NativeHandle (), ((SlotSpineImpl*)slot)->NativeHandle ());
}

void SkeletonClippingSpineImpl::ClipEnd ()
{
  spSkeletonClipping_clipEnd2 (clipper->NativeHandle ());
}

void SkeletonClippingSpineImpl::ClipTriangles (float* vertices, int vertices_count, unsigned short* triangles, int triangles_count, float* uvs, int stride)
{
  spSkeletonClipping_clipTriangles (clipper->NativeHandle (), vertices, vertices_count, triangles, triangles_count, uvs, stride);
}

unsigned int SkeletonClippingSpineImpl::ClippedVerticesCount ()
{
  return clipper->NativeHandle ()->clippedVertices->size / 2; //size is a total number of floats (pairs of x and y)
}

unsigned int SkeletonClippingSpineImpl::ClippedTrianglesCount ()
{
  return clipper->NativeHandle ()->clippedTriangles->size / 3; //triangles is a total number of indices
}

const float* SkeletonClippingSpineImpl::ClippedVertices ()
{
  return clipper->NativeHandle ()->clippedVertices->items;
}

const float* SkeletonClippingSpineImpl::ClippedTexcoords ()
{
  return clipper->NativeHandle ()->clippedUVs->items;
}

const uint16_t* SkeletonClippingSpineImpl::ClippedIndices ()
{
  return clipper->NativeHandle ()->clippedTriangles->items;
}
