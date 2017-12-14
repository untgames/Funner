#ifndef MEDIALIB_SPINE_SHARED_ANIMATION_STATE_HEADER
#define MEDIALIB_SPINE_SHARED_ANIMATION_STATE_HEADER

#include <media/spine/animation_state.h>

#include <object.h>

namespace media
{

namespace spine
{

//forward declarations
class ITrackEntry;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Spine animation state
///////////////////////////////////////////////////////////////////////////////////////////////////
class IAnimationState : virtual public IObject
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Work with tracks
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual ITrackEntry* SetAnimation          (unsigned int track, const char* animation, bool looped) = 0;
    virtual ITrackEntry* SetEmptyAnimation     (unsigned int track, float mix_duration) = 0;
    virtual ITrackEntry* SetEmptyAnimations    (float mix_duration) = 0;
    virtual ITrackEntry* EnqueueAnimation      (unsigned int track, const char* animation, bool looped, float delay) = 0;
    virtual ITrackEntry* EnqueueEmptyAnimation (unsigned int track, float mix_duration, float delay) = 0;
    virtual void         ClearTracks           () = 0;
    virtual void         ClearTrack            (unsigned int track) = 0;
    virtual bool         HasTrackEntry         (unsigned int track) = 0;
    virtual ITrackEntry* TrackEntry            (unsigned int track) = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Register for animation events
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual xtl::connection RegisterEventHandler (AnimationEvent event, const AnimationEventHandler& handler) const = 0;
    virtual xtl::connection RegisterEventHandler (const UserEventHandler& handler) const = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Update (only positive dt allowed)
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void Update (float dt) = 0;
};

}

}

#endif
