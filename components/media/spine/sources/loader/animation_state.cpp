#include "shared.h"

using namespace media::spine;
using namespace media::SPINE_NAMESPACE_NAME;
using namespace SPINE_NAMESPACE_NAME;

namespace
{

//Constants
const char* LOG_NAME = "media.spine.AnimationStateSpineImpl"; //log stream name

void animation_state_listener (spAnimationState* state, spEventType type, spTrackEntry* entry, spEvent* event)
{
  //event was sent after AnimationStateSpineImpl was destroyed
  if (!state->userData)
    return;

  AnimationStateSpineImpl* impl = (AnimationStateSpineImpl*)state->userData;

  media::spine::TrackEntry* track_entry = 0;

  if (entry->userData)
  {
    TrackEntrySpineImpl* track_entry_impl = (TrackEntrySpineImpl*)entry->userData;

    track_entry = &track_entry_impl->ThisTrack ();
  }

  if (!track_entry)
  {
    track_entry = impl->GetActiveTrackEntry (entry);
  }

  try
  {
    switch (type)
    {
      case SP_ANIMATION_START:
        impl->OnEvent (*track_entry, media::spine::AnimationEvent_Start);
        break;
      case SP_ANIMATION_INTERRUPT:
        impl->OnEvent (*track_entry, media::spine::AnimationEvent_Interrupt);
        break;
      case SP_ANIMATION_END:
        impl->OnEvent (*track_entry, media::spine::AnimationEvent_End);
        break;
      case SP_ANIMATION_COMPLETE:
        impl->OnEvent (*track_entry, media::spine::AnimationEvent_Complete);
        break;
      case SP_ANIMATION_DISPOSE:
        impl->OnEvent (*track_entry, media::spine::AnimationEvent_Dispose);
        impl->DisposeTrack (entry);
        break;
      case SP_ANIMATION_EVENT:
        impl->OnEvent (*track_entry, event->data->name, event->intValue, event->floatValue, event->stringValue);
        break;
      default:
        common::Log (LOG_NAME).Printf ("Unknown event occured in track_entry_listener: '%d'", type);
    }
  }
  catch (xtl::exception& e)
  {
    common::Log (LOG_NAME).Printf ("Exception occured in animation_state_listener: '%s'", e.what ());
  }
  catch (...)
  {
    common::Log (LOG_NAME).Printf ("Unknown exception occured in animation_state_listener");
  }
}

}

/*
   Constructor / destructor
*/

AnimationStateSpineImpl::AnimationStateSpineImpl (SpineAtlasPtr in_atlas, SpineSkeletonDataPtr in_skeleton_data, SpineAnimationStateDataPtr in_animation_state_data, SpineAnimationStatePtr in_animation_state)
  : atlas (in_atlas)
  , skeleton_data (in_skeleton_data)
  , animation_state_data (in_animation_state_data)
  , animation_state (in_animation_state)
{
  //we create no more than one AnimationStateSpineImpl per spAnimationState, so we can use pointer to this object as user data (no conflicts).
  animation_state->NativeHandle ()->userData = this;
  animation_state->NativeHandle ()->listener = animation_state_listener;
}

AnimationStateSpineImpl::~AnimationStateSpineImpl ()
{
  for (TrackEntryList::iterator iter = active_tracks.begin (), end = active_tracks.end (); iter != end; ++iter)
  {
    iter->track_entry_impl->Dispose ();
  }

  animation_state->NativeHandle ()->userData = 0;
  animation_state->NativeHandle ()->listener = 0;
}

/*
   Helper methods
*/

AnimationStateSpineImpl::TrackEntryList::iterator AnimationStateSpineImpl::GetActiveTrack (::SPINE_NAMESPACE_NAME::spTrackEntry* track_entry)
{
  for (TrackEntryList::iterator iter = active_tracks.begin (), end = active_tracks.end (); iter != end; ++iter)
  {
    if (iter->track_entry_impl->NativeHandle () == track_entry)
      return iter;
  }

  return active_tracks.insert (active_tracks.end (), TrackEntryDesc (TrackEntryPtr (), TrackEntrySpineImplPtr (new TrackEntrySpineImpl (this, track_entry), false)));
}

/*
   Work with tracks
*/

media::spine::TrackEntryImpl* AnimationStateSpineImpl::SetAnimation (unsigned int track, const char* animation, bool looped)
{
  spTrackEntry* track_entry = spAnimationState_setAnimationByName (animation_state->NativeHandle (), track, animation, looped);

  return GetActiveTrackEntryImplAddRef (track_entry);
}

media::spine::TrackEntryImpl* AnimationStateSpineImpl::SetEmptyAnimation (unsigned int track, float mix_duration)
{
  spTrackEntry* track_entry = spAnimationState_setEmptyAnimation (animation_state->NativeHandle (), track, mix_duration);

  return GetActiveTrackEntryImplAddRef (track_entry);
}

void AnimationStateSpineImpl::SetEmptyAnimations (float mix_duration)
{
  spAnimationState_setEmptyAnimations (animation_state->NativeHandle (), mix_duration);
}

media::spine::TrackEntryImpl* AnimationStateSpineImpl::EnqueueAnimation (unsigned int track, const char* animation, bool looped, float delay)
{
  spTrackEntry* track_entry = spAnimationState_addAnimationByName (animation_state->NativeHandle (), track, animation, looped, delay);

  return GetActiveTrackEntryImplAddRef (track_entry);
}

media::spine::TrackEntryImpl* AnimationStateSpineImpl::EnqueueEmptyAnimation (unsigned int track, float mix_duration, float delay)
{
  spTrackEntry* track_entry = spAnimationState_addEmptyAnimation (animation_state->NativeHandle (), track, mix_duration, delay);

  return GetActiveTrackEntryImplAddRef (track_entry);
}

unsigned int AnimationStateSpineImpl::TracksCount ()
{
  return animation_state->NativeHandle ()->tracksCount;
}

void AnimationStateSpineImpl::ClearTracks ()
{
  spAnimationState_clearTracks (animation_state->NativeHandle ());
}

void AnimationStateSpineImpl::ClearTrack (unsigned int track)
{
  if (track >= TracksCount ())
    return;

  spAnimationState_clearTrack (animation_state->NativeHandle (), track);
}

bool AnimationStateSpineImpl::HasTrackEntry (unsigned int track)
{
  return spAnimationState_getCurrent (animation_state->NativeHandle (), track);
}

media::spine::TrackEntryImpl* AnimationStateSpineImpl::TrackEntry (unsigned int track)
{
  spTrackEntry* track_entry = spAnimationState_getCurrent (animation_state->NativeHandle (), track);

  if (!track_entry)
    return 0;

  return GetActiveTrackEntryImplAddRef (track_entry);
}

media::spine::TrackEntryImpl* AnimationStateSpineImpl::GetActiveTrackEntryImplAddRef (::SPINE_NAMESPACE_NAME::spTrackEntry* track_entry)
{
  media::spine::TrackEntryImpl* return_value = GetActiveTrack (track_entry)->track_entry_impl.get ();

  return_value->AddRef ();

  return return_value;
}

media::spine::TrackEntry* AnimationStateSpineImpl::GetActiveTrackEntry (::SPINE_NAMESPACE_NAME::spTrackEntry* track_entry)
{
  TrackEntryList::iterator iter = GetActiveTrack (track_entry);

  if (!iter->track_entry)
  {
    iter->track_entry = TrackEntryPtr (Wrappers::WrapToNew<media::spine::TrackEntry, media::spine::TrackEntryImpl> (iter->track_entry_impl.get ()));

    //we should add ref to impl object, because it was not captured on previous line
    iter->track_entry_impl->AddRef ();
  }

  return iter->track_entry.get ();
}

void AnimationStateSpineImpl::DisposeTrack (::SPINE_NAMESPACE_NAME::spTrackEntry* track_entry)
{
  for (TrackEntryList::iterator iter = active_tracks.begin (), end = active_tracks.end (); iter != end; ++iter)
  {
    if (iter->track_entry_impl->NativeHandle () == track_entry)
    {
      iter->track_entry_impl->Dispose ();
      active_tracks.erase (iter);
      return;
    }
  }
}

/*
   Update (only positive dt allowed)
*/

void AnimationStateSpineImpl::Update (float dt)
{
  spAnimationState_update (animation_state->NativeHandle (), dt);
}

/*
   Native handle
*/

::SPINE_NAMESPACE_NAME::spAnimationState* AnimationStateSpineImpl::NativeHandle ()
{
  return animation_state->NativeHandle ();
}
