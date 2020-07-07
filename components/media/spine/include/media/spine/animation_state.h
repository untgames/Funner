#ifndef MEDIALIB_SPINE_ANIMATION_STATE_HEADER
#define MEDIALIB_SPINE_ANIMATION_STATE_HEADER

#include <media/spine/track_entry.h>

namespace media
{

namespace spine
{

//forward declarations
class AnimationStateImpl;
class Wrappers;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Spine animation state instance
///////////////////////////////////////////////////////////////////////////////////////////////////
class AnimationState
{
  friend class Wrappers;
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Constructors / destructor / assignment
///////////////////////////////////////////////////////////////////////////////////////////////////
    AnimationState (const AnimationState&);
    ~AnimationState ();

    AnimationState& operator = (const AnimationState&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Work with tracks
///////////////////////////////////////////////////////////////////////////////////////////////////
    media::spine::TrackEntry SetAnimation          (unsigned int track, const char* animation, bool looped);
    media::spine::TrackEntry SetEmptyAnimation     (unsigned int track, float mix_duration);
    void                     SetEmptyAnimations    (float mix_duration);
    media::spine::TrackEntry EnqueueAnimation      (unsigned int track, const char* animation, bool looped, float delay);
    media::spine::TrackEntry EnqueueEmptyAnimation (unsigned int track, float mix_duration, float delay);
    unsigned int             TracksCount           () const;
    void                     ClearTrack            (unsigned int track);
    void                     ClearTracks           ();
    bool                     HasTrackEntry         (unsigned int track) const;
    media::spine::TrackEntry TrackEntry            (unsigned int track) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Register for animation events
///////////////////////////////////////////////////////////////////////////////////////////////////
    xtl::connection RegisterEventHandler (AnimationEvent event, const AnimationEventHandler& handler) const;
    xtl::connection RegisterEventHandler (const UserEventHandler& handler) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Update (only positive dt allowed)
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Update (float dt);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Swap
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Swap (AnimationState&);

  protected:
    AnimationState (AnimationStateImpl*);

  private:
    AnimationStateImpl* impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Swap
///////////////////////////////////////////////////////////////////////////////////////////////////
void swap (AnimationState&, AnimationState&);

}

}

#endif