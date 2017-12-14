#ifndef MEDIALIB_SPINE_SHARED_TRACK_ENTRY_HEADER
#define MEDIALIB_SPINE_SHARED_TRACK_ENTRY_HEADER

#include <media/spine/track_entry.h>

#include <object.h>

namespace media
{

namespace spine
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Spine animation track entry
///////////////////////////////////////////////////////////////////////////////////////////////////
class TrackEntryImpl : virtual public IObject
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Animation parameters
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual const char* Animation         () = 0;
    virtual float       Duration          () = 0;
    virtual float       Tell              () = 0;
    virtual void        Seek              (float seek_to) = 0;

    virtual float       Alpha             () = 0;
    virtual void        SetAlpha          (float alpha) = 0;
    virtual float       AnimationEnd      () = 0;
    virtual void        SetAnimationEnd   (float animation_end) = 0;
    virtual float       AnimationLast     () = 0;
    virtual void        SetAnimationLast  (float animation_last) = 0;
    virtual float       AnimationStart    () = 0;
    virtual void        SetAnimationStart (float animation_start) = 0;
    virtual float       Delay             () = 0;
    virtual void        SetDelay          (float delay) = 0;
    virtual bool        IsLooping         () = 0;
    virtual void        SetLooping        (bool delay) = 0;
    virtual float       TimeScale         () = 0;
    virtual void        SetTimeScale      (float time_scale) = 0;
    virtual float       TrackTime         () = 0;
    virtual void        SetTrackTime      (float track_time) = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Mixing parameters
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual float           MixDuration    () = 0;
    virtual void            SetMixDuration (float mix_duration) = 0;
    virtual float           MixTime        () = 0;
    virtual void            SetMixTime     (float mix_time) = 0;
    virtual TrackEntryImpl* MixingFrom     () = 0;
    virtual TrackEntryImpl* Next           () = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Check if track is already disposed (if track disposed, all other read calls returns default values and set calls are ignored)
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual bool IsDisposed () = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Register for animation events
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual xtl::connection RegisterEventHandler (AnimationEvent event, const AnimationEventHandler& handler) = 0;
    virtual xtl::connection RegisterEventHandler (const UserEventHandler& handler) = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Update (only positive dt allowed)
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void Update (float dt) = 0;
};

}

}

#endif
