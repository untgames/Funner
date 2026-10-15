#include "shared.h"

using namespace syslib;
using namespace syslib::macosx;

namespace
{

const char* LOG_NAME = "syslib.CarbonTimer";

/*
    Resources shared by all timers
*/

struct CarbonTimerResources
{
  EventRef    dummy_event;
  common::Log log;

  CarbonTimerResources ()
    : dummy_event (0), log (LOG_NAME)
  {
    check_event_manager_error (CreateEvent (0, 0, 0, 0, kEventAttributeNone, &dummy_event), "::CreateEvent", "Can't create dummy event");
  }

  ~CarbonTimerResources ()
  {
    if (dummy_event)
      ReleaseEvent (dummy_event);
  }

  private:
    CarbonTimerResources (const CarbonTimerResources&) = delete;
    CarbonTimerResources& operator = (const CarbonTimerResources&) = delete;
};

CarbonTimerResources& GetCarbonTimerResources ()
{
  static CarbonTimerResources resources;

  return resources;
}

/*
    Timer implementation
*/

class CarbonTimer
{
  public:
    CarbonTimer  (size_t period_in_milliseconds, const TimerHandler& handler, void* user_data);
    ~CarbonTimer ();

  private:
    CarbonTimer (const CarbonTimer&); //no impl
    CarbonTimer& operator = (const CarbonTimer&); //no impl

    static void TimerProc (EventLoopTimerRef in_timer, void* user_data);

  private:
    EventLoopTimerRef timer;       //timer
    TimerHandler      handler;     //handler
    void*             user_data;   //user data
};

}

/*
    CarbonTimer
*/

CarbonTimer::CarbonTimer (size_t period_in_milliseconds, const TimerHandler& in_handler, void* in_user_data)
  : timer (0), handler (in_handler), user_data (in_user_data)
{
  try
  {
    GetCarbonTimerResources ();

    float period_in_seconds = period_in_milliseconds / 1000.f;

    check_event_manager_error (InstallEventLoopTimer (GetCurrentEventLoop (), period_in_seconds, period_in_seconds,
                               &TimerProc, this, &timer), "::InstallEventLoopTimer", "Can't install timer");

    if (!timer)
      throw xtl::format_operation_exception ("", "Incorrect timer created, unknown carbon error");
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::CarbonTimer::CarbonTimer");
    throw;
  }
}

CarbonTimer::~CarbonTimer ()
{
  RemoveEventLoopTimer (timer);
}

void CarbonTimer::TimerProc (EventLoopTimerRef in_timer, void* user_data)
{
  CarbonTimer* carbon_timer = (CarbonTimer*)user_data;
  CarbonTimerResources& resources = GetCarbonTimerResources ();

  try
  {
    //The handler may destroy this timer; do not access carbon_timer after calling it.
    carbon_timer->handler (carbon_timer->user_data);

    EventQueueRef current_event_queue (GetCurrentEventQueue ());

    if (!IsEventInQueue (current_event_queue, resources.dummy_event))
      check_event_manager_error (PostEventToQueue (current_event_queue, resources.dummy_event, kEventPriorityLow),
                                 "::PostEventToQueue", "Can't dispatch dummy event");
  }
  catch (xtl::exception& exception)
  {
    resources.log.Printf ("Error at TimerProc: '%s'", exception.what ());
    printf ("Error at TimerProc: '%s'\n", exception.what ());
  }
  catch (...)
  {
    resources.log.Printf ("Error at TimerProc: unknown exception");
    printf ("Error at TimerProc: unknown exception\n");
  }
}

/*
    Timer creation / destruction
*/

timer_t CarbonTimerManager::CreateTimer (size_t period_in_milliseconds, TimerHandler handler, void* user_data)
{
  return (timer_t)new CarbonTimer (period_in_milliseconds, handler, user_data);
}

void CarbonTimerManager::KillTimer (timer_t handle)
{
  if (!handle)
    throw xtl::make_null_argument_exception ("syslib::CarbonTimerManager::KillTimer", "handle");

  CarbonTimer* timer = (CarbonTimer*)handle;

  delete timer;
}
