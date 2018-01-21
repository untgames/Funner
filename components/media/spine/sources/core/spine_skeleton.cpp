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
  impl->ApplyAnimationState (Wrappers::Unwrap<AnimationStateImpl, AnimationState> (animation));
}

/*
   Work with bones
*/

Bone& Skeleton::RootBone ()
{
  return const_cast<media::spine::Bone&> (const_cast<const Skeleton&> (*this).RootBone ());
 }

const Bone& Skeleton::RootBone () const
{
  return impl->RootBone ();
}

Bone* Skeleton::FindBone (const char* name)
{
  return const_cast<media::spine::Bone*> (const_cast<const Skeleton&> (*this).FindBone (name));
}

const Bone* Skeleton::FindBone (const char* name) const
{
  if (!name)
    throw xtl::make_null_argument_exception ("media::spine::Skeleton::FindBone", "name");

  int bone_index = impl->FindBoneIndex (name);

  return bone_index < 0 ? 0 : &impl->Bone (bone_index);
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
  return const_cast<media::spine::Slot&> (const_cast<const Skeleton&> (*this).Slot (index));
}

const media::spine::Slot& Skeleton::Slot (unsigned int index) const
{
  return impl->Slot (index);
}

media::spine::Slot* Skeleton::FindSlot (const char* name)
{
  return const_cast<media::spine::Slot*> (const_cast<const Skeleton&> (*this).FindSlot (name));
}

const media::spine::Slot* Skeleton::FindSlot (const char* name) const
{
  if (!name)
    throw xtl::make_null_argument_exception ("media::spine::Skeleton::FindSlot", "name");

  int slot_index = impl->FindSlotIndex (name);

  return slot_index < 0 ? 0 : &impl->Slot (slot_index);
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

bool Skeleton::SetSkin (const char* skin_name)
{
  //skin name may be null

  return impl->SetSkin (skin_name);
}

bool Skeleton::SetAttachment (const char* slot_name, const char* attachment_name)
{
  if (!slot_name)
    throw xtl::make_null_argument_exception ("media::spine::Skeleton::SetAttachment", "slot_name");

  //attachment name may be null

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
