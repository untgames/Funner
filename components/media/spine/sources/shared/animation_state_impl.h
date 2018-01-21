#ifndef MEDIALIB_SPINE_SHARED_ANIMATION_STATE_HEADER
#define MEDIALIB_SPINE_SHARED_ANIMATION_STATE_HEADER

#include <xtl/signal.h>

#include <media/spine/animation_state.h>
#include <media/spine/track_entry.h>

#include <object.h>

namespace media
{

namespace spine
{

//forward declarations
class TrackEntryImpl;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Spine animation state
///////////////////////////////////////////////////////////////////////////////////////////////////
class AnimationStateImpl : virtual public IObject
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Work with tracks
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual TrackEntryImpl* SetAnimation          (unsigned int track, const char* animation, bool looped) = 0;
    virtual TrackEntryImpl* SetEmptyAnimation     (unsigned int track, float mix_duration) = 0;
    virtual void            SetEmptyAnimations    (float mix_duration) = 0;
    virtual TrackEntryImpl* EnqueueAnimation      (unsigned int track, const char* animation, bool looped, float delay) = 0;
    virtual TrackEntryImpl* EnqueueEmptyAnimation (unsigned int track, float mix_duration, float delay) = 0;
    virtual unsigned int    TracksCount           () = 0;
    virtual void            ClearTracks           () = 0;
    virtual void            ClearTrack            (unsigned int track) = 0;
    virtual bool            HasTrackEntry         (unsigned int track) = 0;
    virtual TrackEntryImpl* TrackEntry            (unsigned int track) = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Update (only positive dt allowed)
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void Update (float dt) = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Register / handling for animation events
///////////////////////////////////////////////////////////////////////////////////////////////////
    xtl::connection RegisterEventHandler (AnimationEvent event, const AnimationEventHandler& handler);
    xtl::connection RegisterEventHandler (const UserEventHandler& handler);

    void OnEvent (const media::spine::TrackEntry& track_entry, AnimationEvent event);
    void OnEvent (const media::spine::TrackEntry& track_entry, const char* event_name, int int_value, float float_value, const char* string_value);

  private:
    typedef xtl::signal<AnimationEventHandler::signature_type> EventSignal;
    typedef xtl::signal<UserEventHandler::signature_type>      UserEventSignal;

  private:
    EventSignal     event_signals [AnimationEvent_Num];
    UserEventSignal user_event_signal;
};

}

}

#endif
