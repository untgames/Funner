#include <xtl/common_exceptions.h>

#include "animation_state_impl.h"
#include "wrappers.h"

using namespace media::spine;

/*
   Register / handling for animation events
*/

xtl::connection AnimationStateImpl::RegisterEventHandler (AnimationEvent event, const AnimationEventHandler& handler)
{
  if (event < 0 || event >= AnimationEvent_Num)
    throw xtl::make_argument_exception ("media::spine::AnimationStateImpl::RegisterEventHandler", "event", event);

  return event_signals [event].connect (handler);
}

xtl::connection AnimationStateImpl::RegisterEventHandler (const UserEventHandler& handler)
{
  return user_event_signal.connect (handler);
}

void AnimationStateImpl::OnEvent (const media::spine::TrackEntry& track_entry, AnimationEvent event)
{
  if (event < 0 || event >= AnimationEvent_Num)
    return;

  event_signals [event] (track_entry, event);
}

void AnimationStateImpl::OnEvent (const media::spine::TrackEntry& track_entry, const char* event_name, int int_value, float float_value, const char* string_value)
{
  user_event_signal (track_entry, event_name, int_value, float_value, string_value);
}
