#ifndef MEDIALIB_SPINE_SHARED_SLOT_HEADER
#define MEDIALIB_SPINE_SHARED_SLOT_HEADER

#include <xtl/shared_ptr.h>

#include <media/spine/attachment.h>
#include <media/spine/bone.h>
#include <media/spine/material.h>
#include <media/spine/slot.h>

#include <object.h>

namespace media
{

namespace spine
{

//forward declarations
class AttachmentImpl;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Slot
///////////////////////////////////////////////////////////////////////////////////////////////////
class SlotImpl : virtual public IObject
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Name
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual const char* Name () = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Params
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual math::vec4f             Color          () = 0;
    virtual media::spine::BlendMode BlendMode      () = 0;
            media::spine::Bone*     Bone           ();
    virtual bool                    HasBone        () = 0;
    virtual media::spine::BoneImpl* CreateBoneImpl () = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Attachment
///////////////////////////////////////////////////////////////////////////////////////////////////
            media::spine::Attachment*     Attachment           ();
    virtual bool                          HasAttachment        () = 0;
    virtual const char*                   AttachmentName       () = 0;
    virtual media::spine::AttachmentImpl* CreateAttachmentImpl () = 0;

  private:
    typedef xtl::shared_ptr<media::spine::Attachment> AttachmentPtr;
    typedef xtl::shared_ptr<media::spine::Bone>       BonePtr;

  private:
    AttachmentPtr attachment;
    BonePtr       bone;
};

}

}

#endif
