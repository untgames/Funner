#ifndef MEDIALIB_SPINE_SHARED_ATTACHMENT_HEADER
#define MEDIALIB_SPINE_SHARED_ATTACHMENT_HEADER

#include <stdint.h>

#include <xtl/shared_ptr.h>

#include <media/spine/attachment.h>
#include <media/spine/material.h>

#include <object.h>

namespace media
{

namespace spine
{

//forward declarations
class SlotImpl;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Spine point attachment
///////////////////////////////////////////////////////////////////////////////////////////////////
class PointAttachmentDataImpl : virtual public IObject
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Point attachment parameters
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual math::vec2f Position () = 0;
    virtual float       Rotation () = 0;
    virtual math::vec4f Color    () = 0;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Spine attachment
///////////////////////////////////////////////////////////////////////////////////////////////////
class AttachmentImpl : virtual public IObject
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Name
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual const char* Name () = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Params
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual const char*                TexturePath   () = 0;
    virtual media::spine::TexcoordWrap TexcoordWrapU () = 0;
    virtual media::spine::TexcoordWrap TexcoordWrapV () = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Get Type and type-specific data.
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual AttachmentType Type () = 0;

            PointAttachmentData*                   TypeSpecificData              (xtl::type<PointAttachmentData>);
    virtual media::spine::PointAttachmentDataImpl* CreatePointAttachmentDataImpl () = 0;
    virtual math::vec4f                            Color                         () = 0;
    virtual unsigned int                           VerticesCount                 () = 0;
    virtual unsigned int                           TrianglesCount                () = 0;
    virtual void                                   ComputeWorldVertices          (SlotImpl* slot, float* vertices, int offset, int stride) = 0;
    virtual const float*                           Texcoords                     () = 0;
    virtual const uint16_t*                        Indices                       () = 0;

  private:
    typedef xtl::shared_ptr<media::spine::PointAttachmentData> PointAttachmentDataPtr;

  private:
    PointAttachmentDataPtr point_attachment_data;
};

}

}

#endif
