#include <xtl/common_exceptions.h>

#include "attachment_impl.h"
#include "bone_impl.h"
#include "slot_impl.h"
#include "wrappers.h"

using namespace media::spine;

/*
   Params
*/

media::spine::Bone* SlotImpl::Bone ()
{
  bool has_bone = HasBone ();

  //bone for slot can not be changed in current spine version, so no need to check if bone was changed
  if (has_bone && !bone)
  {
    bone = BonePtr (Wrappers::WrapToNew<media::spine::Bone, media::spine::BoneImpl> (CreateBoneImpl ()));
  }

  return bone.get ();
}

/*
   Attachment
*/

media::spine::Attachment* SlotImpl::Attachment ()
{
  bool has_attachment = HasAttachment ();

  if (has_attachment)
  {
    //check if attachment needs to be created or changed
    if (!attachment || xtl::xstrcmp (attachment->Name (), AttachmentName ()))
    {
      attachment = AttachmentPtr (Wrappers::WrapToNew<media::spine::Attachment, media::spine::AttachmentImpl> (CreateAttachmentImpl ()));
    }
  }
  else if (attachment)
  {
    attachment.reset ();
  }

  return attachment.get ();
}
