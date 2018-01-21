#include "shared.h"

using namespace media::spine;

/*
   Constructors / destructor / assignment
*/

AnimationStateData::AnimationStateData (const AnimationStateData& source)
  : impl (source.impl)
{
  if (impl)
    impl->AddRef ();
}

AnimationStateData::AnimationStateData (AnimationStateDataImpl* in_impl)
  : impl (in_impl)
{
  if (!impl)
    throw xtl::make_null_argument_exception ("media::spine::AnimationStateData::AnimationStateData (IAnimationStateData*)", "impl");
}

AnimationStateData::~AnimationStateData ()
{
  impl->Release ();
}

AnimationStateData& AnimationStateData::operator = (const AnimationStateData& source)
{
  AnimationStateData (source).Swap (*this);

  return *this;
}

/*
   Create object instance
*/

AnimationState AnimationStateData::CreateAnimationState () const
{
  return Wrappers::Wrap<AnimationState, AnimationStateImpl> (impl->CreateAnimationState ());
}

/*
   Animation mixing
*/

float AnimationStateData::DefaultMix () const
{
  return impl->DefaultMix ();
}

void AnimationStateData::SetDefaultMix (float default_mix)
{
  impl->SetDefaultMix (default_mix);
}

float AnimationStateData::GetMix (const char* animation_from, const char* animation_to) const
{
  static const char* METHOD_NAME = "media::spine::AnimationStateData::GetMix";

  if (!animation_from)
    throw xtl::make_null_argument_exception (METHOD_NAME, "animation_from");

  if (!animation_to)
    throw xtl::make_null_argument_exception (METHOD_NAME, "animation_to");

  return impl->GetMix (animation_from, animation_to);
}

void AnimationStateData::SetMix (const char* animation_from, const char* animation_to, float mix)
{
  static const char* METHOD_NAME = "media::spine::AnimationStateData::SetMix";

  if (!animation_from)
    throw xtl::make_null_argument_exception (METHOD_NAME, "animation_from");

  if (!animation_to)
    throw xtl::make_null_argument_exception (METHOD_NAME, "animation_to");

  impl->SetMix (animation_from, animation_to, mix);
}

/*
   Swap
*/

void AnimationStateData::Swap (AnimationStateData& data)
{
  stl::swap (impl, data.impl);
}

namespace media
{

namespace spine
{

/*
   Swap
*/

void swap (AnimationStateData& data1, AnimationStateData& data2)
{
  data1.Swap (data2);
}

}

}
