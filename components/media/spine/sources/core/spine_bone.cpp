#include "shared.h"

using namespace media::spine;

/*
   Constructors / destructor / assignment
*/

Bone::Bone (const Bone& source)
  : impl (source.impl)
{
  if (impl)
    impl->AddRef ();
}

Bone::Bone (BoneImpl* in_impl)
  : impl (in_impl)
{
  if (!impl)
    throw xtl::make_null_argument_exception ("media::spine::Bone::Bone (IBone*)", "impl");
}

Bone::~Bone ()
{
  impl->Release ();
}

Bone& Bone::operator = (const Bone& source)
{
  Bone (source).Swap (*this);

  return *this;
}

/*
   Name
*/

const char* Bone::Name () const
{
  return impl->Name ();
}

/*
   Transform
*/

void Bone::SetPosition (const math::vec2f& position)
{
  impl->SetPosition (position);
}

void Bone::SetPosition (float position_x, float position_y)
{
  impl->SetPosition (position_x, position_y);
}

math::vec2f Bone::Position () const
{
  return impl->Position ();
}

void Bone::SetRotation (float rotation)
{
  impl->SetRotation (rotation);
}

float Bone::Rotation () const
{
  return impl->Rotation ();
}

void Bone::SetScale (const math::vec2f& scale)
{
  impl->SetScale (scale);
}

void Bone::SetScale (float scale_x, float scale_y)
{
  impl->SetScale (scale_x, scale_y);
}

math::vec2f Bone::Scale () const
{
  return impl->Scale ();
}

void Bone::SetShear (const math::vec2f& shear)
{
  impl->SetShear (shear);
}

void Bone::SetShear (float shear_x, float shear_y)
{
  impl->SetShear (shear_x, shear_y);
}

math::vec2f Bone::Shear () const
{
  return impl->Shear ();
}

math::vec2f Bone::WorldPosition () const
{
  return impl->WorldPosition ();
}

math::vec2f Bone::WorldRotation () const
{
  return impl->WorldRotation ();
}

math::vec2f Bone::WorldScale () const
{
  return impl->WorldScale ();
}

/*
   Helpers for converting between coordinate spaces
*/

math::vec2f Bone::WorldToLocal (const math::vec2f& position) const
{
  return impl->WorldToLocal (position);
}

math::vec2f Bone::WorldToLocal (float position_x, float position_y) const
{
  return impl->WorldToLocal (position_x, position_y);
}

float Bone::WorldToLocal (float rotation) const
{
  return impl->WorldToLocal (rotation);
}

math::vec2f Bone::LocalToWorld (const math::vec2f& position) const
{
  return impl->LocalToWorld (position);
}

math::vec2f Bone::LocalToWorld (float position_x, float position_y) const
{
  return impl->LocalToWorld (position_x, position_y);
}

float Bone::LocalToWorld (float rotation) const
{
  return impl->LocalToWorld (rotation);
}

/*
   Linked bones, returned object is owned by callee.
*/

Bone* Bone::Parent ()
{
  return const_cast<media::spine::Bone*> (const_cast<const Bone&> (*this).Parent ());
}

const Bone* Bone::Parent () const
{
  return impl->Parent ();
}

unsigned int Bone::ChildrenCount () const
{
  return impl->ChildrenCount ();
}

Bone& Bone::Child (unsigned int index)
{
  return const_cast<media::spine::Bone&> (const_cast<const Bone&> (*this).Child (index));
}

const Bone& Bone::Child (unsigned int index) const
{
  return impl->Child (index);
}

/*
   Swap
*/

void Bone::Swap (Bone& bone)
{
  stl::swap (impl, bone.impl);
}

namespace media
{

namespace spine
{

/*
   Swap
*/

void swap (Bone& bone1, Bone& bone2)
{
  bone1.Swap (bone2);
}

}

}
