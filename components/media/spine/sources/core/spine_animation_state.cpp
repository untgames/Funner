#include "shared.h"

using namespace media::spine;

/*
   Constructors / destructor / assignment
*/

AnimationState::AnimationState (const AnimationState& source)
  : impl (source.impl)
{
  if (impl)
    impl->AddRef ();
}

AnimationState::AnimationState (AnimationStateImpl* in_impl)
  : impl (in_impl)
{
  if (!impl)
    throw xtl::make_null_argument_exception ("media::spine::AnimationState::AnimationState (IAnimationState*)", "impl");
}

AnimationState::~AnimationState ()
{
  impl->Release ();
}

AnimationState& AnimationState::operator = (const AnimationState& source)
{
  AnimationState (source).Swap (*this);

  return *this;
}

/*
   Work with tracks
*/

TrackEntry AnimationState::SetAnimation (unsigned int track, const char* animation, bool looped)
{
  if (!animation)
    throw xtl::make_null_argument_exception ("media::spine::AnimationState::SetAnimation", "animation");

  return Wrappers::Wrap<media::spine::TrackEntry, TrackEntryImpl> (impl->SetAnimation (track, animation, looped));
}

TrackEntry AnimationState::SetEmptyAnimation (unsigned int track, float mix_duration)
{
  return Wrappers::Wrap<media::spine::TrackEntry, TrackEntryImpl> (impl->SetEmptyAnimation (track, mix_duration));
}

void AnimationState::SetEmptyAnimations (float mix_duration)
{
  impl->SetEmptyAnimations (mix_duration);
}

TrackEntry AnimationState::EnqueueAnimation (unsigned int track, const char* animation, bool looped, float delay)
{
  if (!animation)
    throw xtl::make_null_argument_exception ("media::spine::AnimationState::EnqueueAnimation", "animation");

  return Wrappers::Wrap<media::spine::TrackEntry, TrackEntryImpl> (impl->EnqueueAnimation (track, animation, looped, delay));
}

TrackEntry AnimationState::EnqueueEmptyAnimation (unsigned int track, float mix_duration, float delay)
{
  return Wrappers::Wrap<media::spine::TrackEntry, TrackEntryImpl> (impl->EnqueueEmptyAnimation (track, mix_duration, delay));
}

unsigned int AnimationState::TracksCount () const
{
  return impl->TracksCount ();
}

void AnimationState::ClearTracks ()
{
  impl->ClearTracks ();
}

void AnimationState::ClearTrack (unsigned int track)
{
  impl->ClearTrack (track);
}

bool AnimationState::HasTrackEntry (unsigned int track) const
{
  return impl->HasTrackEntry (track);
}

TrackEntry AnimationState::TrackEntry (unsigned int track) const
{
  return Wrappers::Wrap<media::spine::TrackEntry, TrackEntryImpl> (impl->TrackEntry (track));
}

/*
   Register for animation events
*/

xtl::connection AnimationState::RegisterEventHandler (AnimationEvent event, const AnimationEventHandler& handler) const
{
  return impl->RegisterEventHandler (event, handler);
}

xtl::connection AnimationState::RegisterEventHandler (const UserEventHandler& handler) const
{
  return impl->RegisterEventHandler (handler);
}

/*
   Update (only positive dt allowed)
*/

void AnimationState::Update (float dt)
{
  impl->Update (dt);
}

/*
   Swap
*/

void AnimationState::Swap (AnimationState& state)
{
  stl::swap (impl, state.impl);
}

namespace media
{

namespace spine
{

/*
   Swap
*/

void swap (AnimationState& state1, AnimationState& state2)
{
  state1.Swap (state2);
}

}

}
