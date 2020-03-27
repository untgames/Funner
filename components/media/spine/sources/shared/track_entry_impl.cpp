#include <xtl/common_exceptions.h>

#include "track_entry_impl.h"
#include "wrappers.h"

using namespace media::spine;

/*
   Register / handling for animation events
*/

xtl::connection TrackEntryImpl::RegisterEventHandler (AnimationEvent event, const AnimationEventHandler& handler)
{
  if (event < 0 || event >= AnimationEvent_Num)
    throw xtl::make_argument_exception ("media::spine::TrackEntryImpl::RegisterEventHandler", "event", event);

  return event_signals [event].connect (handler);
}

xtl::connection TrackEntryImpl::RegisterEventHandler (const UserEventHandler& handler)
{
  return user_event_signal.connect (handler);
}

void TrackEntryImpl::OnEvent (AnimationEvent event)
{
  if (event < 0 || event >= AnimationEvent_Num)
    return;

  event_signals [event] (ThisTrack (), event);
}

void TrackEntryImpl::OnEvent (const char* event_name, int int_value, float float_value, const char* string_value)
{
  user_event_signal (ThisTrack (), event_name, int_value, float_value, string_value);
}
