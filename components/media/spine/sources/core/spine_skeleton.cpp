#include "shared.h"

using namespace media::spine;

/*
   Constructors / destructor / assignment
*/

Skeleton::Skeleton (const Skeleton& source)
  : impl (source.impl)
{
  if (impl)
    impl->AddRef ();
}

Skeleton::Skeleton (SkeletonImpl* in_impl)
  : impl (in_impl)
{
  if (!impl)
    throw xtl::make_null_argument_exception ("media::spine::Skeleton::Skeleton (ISkeleton*)", "impl");
}

Skeleton::~Skeleton ()
{
  impl->Release ();
}

Skeleton& Skeleton::operator = (const Skeleton& source)
{
  Skeleton (source).Swap (*this);

  return *this;
}

/*
   Animating
*/

void Skeleton::SetToSetupPose ()
{
  impl->SetToSetupPose ();
}

void Skeleton::Apply (const AnimationState& animation)
{
  impl->ApplyAnimationState (animation);
}

/*
   Work with bones
*/

Bone& Skeleton::RootBone ()
{
  //TODO
  throw xtl::make_not_implemented_exception ("Skeleton::RootBone");
}

const Bone& Skeleton::RootBone () const
{
  //TODO
  throw xtl::make_not_implemented_exception ("Skeleton::RootBone");
}

Bone* Skeleton::FindBone (const char* name)
{
  //TODO
  return 0;
}

const Bone* Skeleton::FindBone (const char* name) const
{
  //TODO
  return 0;
}

/*
   Work with slots
*/

unsigned int Skeleton::SlotsCount () const
{
  return impl->SlotsCount ();
}

media::spine::Slot& Skeleton::Slot (unsigned int index)
{
  //TODO
  throw xtl::make_not_implemented_exception ("Skeleton::RootBone");
}

const media::spine::Slot& Skeleton::Slot (unsigned int index) const
{
  //TODO
  throw xtl::make_not_implemented_exception ("Skeleton::RootBone");
}

media::spine::Slot* Skeleton::FindSlot (const char* name)
{
  //TODO
  return 0;
}

const media::spine::Slot* Skeleton::FindSlot (const char* name) const
{
  //TODO
  return 0;
}

/*
   Rendering
*/

unsigned int Skeleton::MeshesCount () const
{
  return impl->MeshesCount ();
}

media::geometry::Mesh Skeleton::Mesh (unsigned int mesh_index) const
{
  return impl->Mesh (mesh_index);
}

Material Skeleton::Material (const char* name) const
{
  return Wrappers::Wrap<media::spine::Material, MaterialImpl> (impl->Material (name));
}

/*
   Change skin (use 0 to set default skin)
*/

const char* Skeleton::Skin () const
{
  return impl->Skin ();
}

void Skeleton::SetSkin (const char* skin_name)
{
  impl->SetSkin (skin_name);
}

bool Skeleton::SetAttachment (const char* slot_name, const char* attachment_name)
{
  return impl->SetAttachment (slot_name, attachment_name);
}

/*
   Update transform after animation/bones manipulation
*/

void Skeleton::UpdateWorldTransform ()
{
  impl->UpdateWorldTransform ();
}

/*
   Swap
*/

void Skeleton::Swap (Skeleton& skeleton)
{
  stl::swap (impl, skeleton.impl);
}

namespace media
{

namespace spine
{

/*
   Swap
*/

void swap (Skeleton& skeleton1, Skeleton& skeleton2)
{
  skeleton1.Swap (skeleton2);
}

}

}
