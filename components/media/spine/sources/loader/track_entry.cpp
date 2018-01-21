#include "shared.h"

using namespace SPINE_NAMESPACE_NAME;
using namespace media::SPINE_NAMESPACE_NAME;

namespace
{

//Constants
const char* LOG_NAME = "media.spine.TrackEntrySpineImpl"; //log stream name

void track_entry_listener (spAnimationState* state, spEventType type, spTrackEntry* entry, spEvent* event)
{
  //track was already disposed
  if (!entry->userData)
    return;

  TrackEntrySpineImpl* impl = (TrackEntrySpineImpl*)entry->userData;

  try
  {
    switch (type)
    {
      case SP_ANIMATION_START:
        impl->OnEvent (media::spine::AnimationEvent_Start);
        break;
      case SP_ANIMATION_INTERRUPT:
        impl->OnEvent (media::spine::AnimationEvent_Interrupt);
        break;
      case SP_ANIMATION_END:
        impl->OnEvent (media::spine::AnimationEvent_End);
        break;
      case SP_ANIMATION_COMPLETE:
        impl->OnEvent (media::spine::AnimationEvent_Complete);
        break;
      case SP_ANIMATION_DISPOSE:
        impl->OnEvent (media::spine::AnimationEvent_Dispose);
        impl->Dispose ();
        break;
      case SP_ANIMATION_EVENT:
        impl->OnEvent (event->data->name, event->intValue, event->floatValue, event->stringValue);
        break;
      default:
        common::Log (LOG_NAME).Printf ("Unknown event occured in track_entry_listener: '%d'", type);
    }
  }
  catch (xtl::exception& e)
  {
    common::Log (LOG_NAME).Printf ("Exception occured in track_entry_listener: '%s'", e.what ());
  }
  catch (...)
  {
    common::Log (LOG_NAME).Printf ("Unknown exception occured in track_entry_listener");
  }
}

}

/*
   Constructor / destructor
*/

TrackEntrySpineImpl::TrackEntrySpineImpl (AnimationStateSpineImpl* in_animation_state_impl, ::SPINE_NAMESPACE_NAME::spTrackEntry* in_track_entry)
  : animation_state_impl (in_animation_state_impl)
  , this_track_entry (0)
  , track_entry (in_track_entry)
{
  //we create no more than one TrackEntrySpineImpl per spTrackEntry, so we can use pointer to this object as user data (no conflicts).
  track_entry->userData = this;
  track_entry->listener = track_entry_listener;
}

TrackEntrySpineImpl::~TrackEntrySpineImpl ()
{
  Dispose ();
}

/*
   Animation parameters
*/

const char* TrackEntrySpineImpl::Animation ()
{
  CheckDisposed ("media::spine::TrackEntrySpineImpl::Animation");

  return track_entry->animation->name;
}

float TrackEntrySpineImpl::Duration ()
{
  CheckDisposed ("media::spine::TrackEntrySpineImpl::Duration");

  return track_entry->animation->duration;
}

unsigned int TrackEntrySpineImpl::TrackIndex ()
{
  CheckDisposed ("media::spine::TrackEntrySpineImpl::TrackIndex");

  return track_entry->trackIndex;
}

float TrackEntrySpineImpl::Alpha ()
{
  CheckDisposed ("media::spine::TrackEntrySpineImpl::Alpha");

  return track_entry->alpha;
}

void TrackEntrySpineImpl::SetAlpha (float alpha)
{
  CheckDisposed ("media::spine::TrackEntrySpineImpl::SetAlpha");

  track_entry->alpha = alpha;
}

float TrackEntrySpineImpl::AnimationEnd ()
{
  CheckDisposed ("media::spine::TrackEntrySpineImpl::AnimationEnd");

  return track_entry->animationEnd;
}

void TrackEntrySpineImpl::SetAnimationEnd (float animation_end)
{
  CheckDisposed ("media::spine::TrackEntrySpineImpl::SetAnimationEnd");

  track_entry->animationEnd = animation_end;
}

float TrackEntrySpineImpl::AnimationLast ()
{
  CheckDisposed ("media::spine::TrackEntrySpineImpl::AnimationLast");

  return track_entry->animationLast;
}

void TrackEntrySpineImpl::SetAnimationLast (float animation_last)
{
  CheckDisposed ("media::spine::TrackEntrySpineImpl::SetAnimationLast");

  track_entry->animationLast = animation_last;
}

float TrackEntrySpineImpl::AnimationStart ()
{
  CheckDisposed ("media::spine::TrackEntrySpineImpl::AnimationStart");

  return track_entry->animationStart;
}

void TrackEntrySpineImpl::SetAnimationStart (float animation_start)
{
  CheckDisposed ("media::spine::TrackEntrySpineImpl::AnimationStart");

  track_entry->animationStart = animation_start;
}

float TrackEntrySpineImpl::Delay ()
{
  CheckDisposed ("media::spine::TrackEntrySpineImpl::Delay");

  return track_entry->delay;
}

void TrackEntrySpineImpl::SetDelay (float delay)
{
  CheckDisposed ("media::spine::TrackEntrySpineImpl::SetDelay");

  track_entry->delay = delay;
}

bool TrackEntrySpineImpl::IsLooping ()
{
  CheckDisposed ("media::spine::TrackEntrySpineImpl::IsLooping");

  return track_entry->loop;
}

void TrackEntrySpineImpl::SetLooping (bool is_looping)
{
  CheckDisposed ("media::spine::TrackEntrySpineImpl::SetLooping");

  track_entry->loop = is_looping;
}

float TrackEntrySpineImpl::TimeScale ()
{
  CheckDisposed ("media::spine::TrackEntrySpineImpl::TimeScale");

  return track_entry->timeScale;
}

void TrackEntrySpineImpl::SetTimeScale (float time_scale)
{
  CheckDisposed ("media::spine::TrackEntrySpineImpl::SetTimeScale");

  track_entry->timeScale = time_scale;
}

float TrackEntrySpineImpl::TrackTime ()
{
  CheckDisposed ("media::spine::TrackEntrySpineImpl::TrackTime");

  return track_entry->trackTime;
}

void TrackEntrySpineImpl::SetTrackTime (float track_time)
{
  CheckDisposed ("media::spine::TrackEntrySpineImpl::SetTrackTime");

  track_entry->trackTime = track_time;
}

/*
   Mixing parameters
*/

float TrackEntrySpineImpl::MixDuration ()
{
  CheckDisposed ("media::spine::TrackEntrySpineImpl::MixDuration");

  return track_entry->mixDuration;
}

void TrackEntrySpineImpl::SetMixDuration (float mix_duration)
{
  CheckDisposed ("media::spine::TrackEntrySpineImpl::SetMixDuration");

  track_entry->mixDuration = mix_duration;
}

float TrackEntrySpineImpl::MixTime ()
{
  CheckDisposed ("media::spine::TrackEntrySpineImpl::MixTime");

  return track_entry->mixTime;
}

void TrackEntrySpineImpl::SetMixTime (float mix_time)
{
  CheckDisposed ("media::spine::TrackEntrySpineImpl::SetMixTime");

  track_entry->mixTime = mix_time;
}

media::spine::TrackEntry* TrackEntrySpineImpl::MixingFrom ()
{
  CheckDisposed ("media::spine::TrackEntrySpineImpl::MixingFrom");

  if (!track_entry->mixingFrom)
    return 0;

  return animation_state_impl->GetActiveTrackEntry (track_entry->mixingFrom);
}

media::spine::TrackEntry* TrackEntrySpineImpl::Next ()
{
  CheckDisposed ("media::spine::TrackEntrySpineImpl::Next");

  if (!track_entry->next)
    return 0;

  return animation_state_impl->GetActiveTrackEntry (track_entry->next);
}

/*
   Check if track is already disposed (if track disposed, all other read calls returns default values and set calls are ignored)
*/

bool TrackEntrySpineImpl::IsDisposed ()
{
  return !track_entry;
}

void TrackEntrySpineImpl::Dispose ()
{
  if (!track_entry)
    return;

  track_entry->userData = 0;
  track_entry->listener = 0;
  track_entry = 0;
}

/*
   Get track entry object pointer
*/

media::spine::TrackEntry& TrackEntrySpineImpl::ThisTrack ()
{
  if (!this_track_entry)
    this_track_entry = animation_state_impl->GetActiveTrackEntry (track_entry);

  return *this_track_entry;
}

/*
   Native spine handle
*/

::SPINE_NAMESPACE_NAME::spTrackEntry* TrackEntrySpineImpl::NativeHandle ()
{
  return track_entry;
}

/*
   Helper methods
*/

void TrackEntrySpineImpl::CheckDisposed (const char* method_name)
{
  if (!track_entry)
    throw xtl::format_operation_exception (method_name, "Track is disposed");
}
