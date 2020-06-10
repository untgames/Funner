#include "shared.h"

using namespace media::spine;

/*
   Constructors / destructor / assignment
*/

Slot::Slot (const Slot& source)
  : impl (source.impl)
{
  if (impl)
    impl->AddRef ();
}

Slot::Slot (SlotImpl* in_impl)
  : impl (in_impl)
{
  if (!impl)
    throw xtl::make_null_argument_exception ("media::spine::Slot::Slot (ISlot*)", "impl");
}

Slot::~Slot ()
{
  impl->Release ();
}

Slot& Slot::operator = (const Slot& source)
{
  Slot (source).Swap (*this);

  return *this;
}

/*
   Name
*/

const char* Slot::Name () const
{
  return impl->Name ();
}

/*
   Params, returned object is owned by callee.
*/

math::vec4f Slot::Color () const
{
  return impl->Color ();
}

math::vec4f Slot::DarkColor () const
{
  return impl->DarkColor ();
}

const Bone* Slot::Bone () const
{
  return impl->Bone ();
}

/*
   Attachment, returned object is owned by callee.
*/

Attachment* Slot::Attachment ()
{
  return const_cast<media::spine::Attachment*> (const_cast<const Slot&> (*this).Attachment ());
}

const Attachment* Slot::Attachment () const
{
  return impl->Attachment ();
}

/*
   Swap
*/

void Slot::Swap (Slot& slot)
{
  stl::swap (impl, slot.impl);
}

namespace media
{

namespace spine
{

/*
   Swap
*/

void swap (Slot& slot1, Slot& slot2)
{
  slot1.Swap (slot2);
}

}

}
