#ifndef MEDIALIB_SPINE_SHARED_SKELETON_CLIPPING_HEADER
#define MEDIALIB_SPINE_SHARED_SKELETON_CLIPPING_HEADER

#include <object.h>

namespace media
{

namespace spine
{

//forward declarations
class AttachmentImpl;
class SlotImpl;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Spine skeleton clipping
///////////////////////////////////////////////////////////////////////////////////////////////////
class SkeletonClippingImpl : virtual public IObject
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Clipping
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual bool IsClipping () = 0;
    virtual void ClipStart  (SlotImpl* slot, AttachmentImpl* clip_attachment) = 0;
    virtual void ClipEnd    (SlotImpl* slot) = 0;        //ends clipping if this slot is a final clipping slot
    virtual void ClipEnd    () = 0;                      //force ends clipping

    virtual void            ClipTriangles         (float* vertices, int vertices_count, unsigned short* triangles, int triangles_count, float* uvs, int stride) = 0;
    virtual unsigned int    ClippedVerticesCount  () = 0;
    virtual unsigned int    ClippedTrianglesCount () = 0;
    virtual const float*    ClippedVertices       () = 0;
    virtual const float*    ClippedTexcoords      () = 0;
    virtual const uint16_t* ClippedIndices        () = 0;
};

}

}

#endif
