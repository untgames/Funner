#include "shared.h"

using namespace media::spine;

/*
   Constructors / destructor / assignment
*/

PointAttachmentData::PointAttachmentData (const PointAttachmentData& source)
  : impl (source.impl)
{
  if (impl)
    impl->AddRef ();
}

PointAttachmentData::PointAttachmentData (PointAttachmentDataImpl* in_impl)
  : impl (in_impl)
{
  if (!impl)
    throw xtl::make_null_argument_exception ("media::spine::PointAttachmentData::PointAttachmentData (IPointAttachmentData*)", "impl");
}

PointAttachmentData::~PointAttachmentData ()
{
  impl->Release ();
}

PointAttachmentData& PointAttachmentData::operator = (const PointAttachmentData& source)
{
  PointAttachmentData (source).Swap (*this);

  return *this;
}

/*
   Point attachment parameters
*/

math::vec2f PointAttachmentData::Position () const
{
  return impl->Position ();
}

float PointAttachmentData::Rotation () const
{
  return impl->Rotation ();
}

math::vec4f PointAttachmentData::Color () const
{
  return impl->Color ();
}

/*
   Swap
*/

void PointAttachmentData::Swap (PointAttachmentData& attachment)
{
  stl::swap (impl, attachment.impl);
}

namespace media
{

namespace spine
{

/*
   Swap
*/

void swap (PointAttachmentData& attachment1, PointAttachmentData& attachment2)
{
  attachment1.Swap (attachment2);
}

}

}
