#include "shared.h"

using namespace media::spine;

/*
   Constructors / destructor / assignment
*/

TrackEntry::TrackEntry (const TrackEntry& source)
  : impl (source.impl)
{
  if (impl)
    impl->AddRef ();
}

TrackEntry::TrackEntry (TrackEntryImpl* in_impl)
  : impl (in_impl)
{
  if (!impl)
    throw xtl::make_null_argument_exception ("media::spine::TrackEntry::TrackEntry (ITrackEntry*)", "impl");
}

TrackEntry::~TrackEntry ()
{
  impl->Release ();
}

TrackEntry& TrackEntry::operator = (const TrackEntry& source)
{
  TrackEntry (source).Swap (*this);

  return *this;
}

/*
   Animation parameters
*/

const char* TrackEntry::Animation () const
{
  return impl->Animation ();
}

float TrackEntry::Duration () const
{
  return impl->Duration ();
}

unsigned int TrackEntry::TrackIndex () const
{
  return impl->TrackIndex ();
}

float TrackEntry::Alpha () const
{
  return impl->Alpha ();
}

void TrackEntry::SetAlpha (float alpha)
{
  impl->SetAlpha (alpha);
}

float TrackEntry::AnimationEnd () const
{
  return impl->AnimationEnd ();
}

void TrackEntry::SetAnimationEnd (float animation_end)
{
  impl->SetAnimationEnd (animation_end);
}

float TrackEntry::AnimationLast () const
{
  return impl->AnimationLast ();
}

void TrackEntry::SetAnimationLast (float animation_last)
{
  impl->SetAnimationLast (animation_last);
}

float TrackEntry::AnimationStart () const
{
  return impl->AnimationStart ();
}

void TrackEntry::SetAnimationStart (float animation_start)
{
  impl->SetAnimationStart (animation_start);
}

float TrackEntry::Delay () const
{
  return impl->Delay ();
}

void TrackEntry::SetDelay (float delay)
{
  impl->SetDelay (delay);
}

bool TrackEntry::IsLooping () const
{
  return impl->IsLooping ();
}

void TrackEntry::SetLooping (bool is_looping)
{
  impl->SetLooping (is_looping);
}

float TrackEntry::TimeScale () const
{
  return impl->TimeScale ();
}

void TrackEntry::SetTimeScale (float time_scale)
{
  impl->SetTimeScale (time_scale);
}

float TrackEntry::TrackTime () const
{
  return impl->TrackTime ();
}

void TrackEntry::SetTrackTime (float track_time)
{
  impl->SetTrackTime (track_time);
}

//tracks blending (available for spine 3.8 or newer)
media::spine::MixBlend TrackEntry::MixBlend () const
{
  return impl->MixBlend ();
}

void TrackEntry::SetMixBlend (media::spine::MixBlend mix_blend)
{
  impl->SetMixBlend (mix_blend);
}

bool TrackEntry::HoldPrevious () const
{
  return impl->HoldPrevious ();
}

void TrackEntry::SetHoldPrevious (bool hold)
{
  impl->SetHoldPrevious (hold);
}

/*
   Mixing parameters
*/

float TrackEntry::MixDuration () const
{
  return impl->MixDuration ();
}

void TrackEntry::SetMixDuration (float mix_duration)
{
  impl->SetMixDuration (mix_duration);
}

float TrackEntry::MixTime () const
{
  return impl->MixTime ();
}

void TrackEntry::SetMixTime (float mix_time)
{
  impl->SetMixTime (mix_time);
}

TrackEntry* TrackEntry::MixingFrom () const
{
  return impl->MixingFrom ();
}

TrackEntry* TrackEntry::Next () const
{
  return impl->Next ();
}

/*
   Check if track is already disposed (if track disposed, all other read calls returns default values and set calls are ignored)
*/

bool TrackEntry::IsDisposed () const
{
  return impl->IsDisposed ();
}

/*
   Register for animation events
*/

xtl::connection TrackEntry::RegisterEventHandler (AnimationEvent event, const AnimationEventHandler& handler) const
{
  return impl->RegisterEventHandler (event, handler);
}

xtl::connection TrackEntry::RegisterEventHandler (const UserEventHandler& handler) const
{
  return impl->RegisterEventHandler (handler);
}

/*
   Swap
*/

void TrackEntry::Swap (TrackEntry& track)
{
  stl::swap (impl, track.impl);
}

namespace media
{

namespace spine
{

/*
   Swap
*/

void swap (TrackEntry& track1, TrackEntry& track2)
{
  track1.Swap (track2);
}

}

}
