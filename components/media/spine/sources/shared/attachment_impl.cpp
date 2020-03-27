#include <xtl/common_exceptions.h>

#include "attachment_impl.h"
#include "wrappers.h"

using namespace media::spine;

/*
   Attachment
*/

PointAttachmentData* AttachmentImpl::TypeSpecificData (xtl::type<PointAttachmentData>)
{
  if (Type () != AttachmentType_Point)
    return 0;

  if (!point_attachment_data)
    point_attachment_data = PointAttachmentDataPtr (Wrappers::WrapToNew<media::spine::PointAttachmentData, media::spine::PointAttachmentDataImpl> (CreatePointAttachmentDataImpl ()));

  return point_attachment_data.get ();
}
