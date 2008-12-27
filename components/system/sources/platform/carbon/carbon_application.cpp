#include "shared.h"

using namespace syslib;

namespace
{

const double RECEIVE_EVENT_TIMEOUT = 0.000000001;

}

/*
    Работа с очередью сообщений
*/

bool Platform::IsMessageQueueEmpty ()
{
  return GetNumEventsInQueue (GetCurrentEventQueue ()) == 0;
}

void Platform::DoNextEvent ()
{
  EventRef event;

  ReceiveNextEvent (0, 0, RECEIVE_EVENT_TIMEOUT, true, &event);

  if (!event)
    return;

  if (GetEventClass (event) == kEventClassApplication)
    if (GetEventKind (event) == kEventAppQuit)
      syslib::Application::Exit (0);

  try
  {
    OSStatus operation_result = SendEventToEventTarget (event, GetEventDispatcherTarget ());

    if (operation_result != eventNotHandledErr)
      check_event_manager_error (operation_result, "::SendEventToEventTarget", "Can't dispatch event");
  }
  catch (xtl::exception& exception)
  {
    ReleaseEvent (event);

    throw xtl::format_operation_exception ("syslib::CarbonPlatform::DoNextEvent", "%s", exception.what ());
  }

  ReleaseEvent (event);
}

void Platform::WaitMessage ()
{
  EventRef next_event;

  check_event_manager_error (ReceiveNextEvent (0, 0, kEventDurationForever, false, &next_event),
                             "syslib::CarbonPlatform::WaitMessage", "Can't wait for next message, error at '::ReceiveNextEvent'");
}

void Platform::UpdateMessageQueue ()
{
  EventRef next_event;

  ReceiveNextEvent (0, 0, RECEIVE_EVENT_TIMEOUT, false, &next_event);
}
