#ifndef MEDIALIB_SPINE_TRACK_ENTRY_HEADER
#define MEDIALIB_SPINE_TRACK_ENTRY_HEADER

#include <xtl/functional_fwd>

namespace media
{

namespace spine
{

//forward declarations
class TrackEntry;
class TrackEntryImpl;
class Wrappers;

//Possible animation event types
enum AnimationEvent
{
  AnimationEvent_Start,
  AnimationEvent_Interrupt,
  AnimationEvent_End,
  AnimationEvent_Complete,
  AnimationEvent_Dispose,

  AnimationEvent_Num
};

typedef xtl::function<void (const TrackEntry& track, AnimationEvent event)> AnimationEventHandler;
typedef xtl::function<void (const TrackEntry& track, const char* event_name, int int_value, float float_value, const char* string_value)> UserEventHandler;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Spine animation track entry
///////////////////////////////////////////////////////////////////////////////////////////////////
class TrackEntry
{
  friend class Wrappers;
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Constructors / destructor / assignment
///////////////////////////////////////////////////////////////////////////////////////////////////
    TrackEntry  (const TrackEntry&);
    ~TrackEntry ();

    TrackEntry& operator = (const TrackEntry&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Animation parameters
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char*  Animation  () const;
    float        Duration   () const;
    unsigned int TrackIndex () const;

    float       Alpha             () const;
    void        SetAlpha          (float alpha);
    float       AnimationEnd      () const;
    void        SetAnimationEnd   (float animation_end);
    float       AnimationLast     () const;
    void        SetAnimationLast  (float animation_last);
    float       AnimationStart    () const;
    void        SetAnimationStart (float animation_start);
    float       Delay             () const;
    void        SetDelay          (float delay);
    bool        IsLooping         () const;
    void        SetLooping        (bool is_looping);
    float       TimeScale         () const;
    void        SetTimeScale      (float time_scale);
    float       TrackTime         () const;
    void        SetTrackTime      (float track_time);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Mixing parameters, returned object is owned by callee.
///////////////////////////////////////////////////////////////////////////////////////////////////
    float       MixDuration    () const;
    void        SetMixDuration (float mix_duration);
    float       MixTime        () const;
    void        SetMixTime     (float mix_time);
    TrackEntry* MixingFrom     () const;
    TrackEntry* Next           () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Check if track is already disposed (if track disposed, all other read calls returns default values and set calls are ignored)
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool IsDisposed () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Register for animation events
///////////////////////////////////////////////////////////////////////////////////////////////////
    xtl::connection RegisterEventHandler (AnimationEvent event, const AnimationEventHandler& handler) const;
    xtl::connection RegisterEventHandler (const UserEventHandler& handler) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Swap
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Swap (TrackEntry&);

  protected:
    TrackEntry (TrackEntryImpl*);

  private:
    TrackEntryImpl* impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Swap
///////////////////////////////////////////////////////////////////////////////////////////////////
void swap (TrackEntry&, TrackEntry&);

}

}

#endif
