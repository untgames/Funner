#include "shared.h"

using namespace syslib;

namespace
{

const char* LOG_NAME = "syslib.CarbonTimer";

/*
    Описание реализации таймера
*/

class CarbonTimer
{
  public:
    typedef Platform::TimerHandler TimerHandler;

    CarbonTimer  (size_t period_in_milliseconds, const TimerHandler& handler, void* user_data);
    ~CarbonTimer ();

  private:
    CarbonTimer (const CarbonTimer&); //no impl
    CarbonTimer& operator = (const CarbonTimer&); //no impl

    static void TimerProc (EventLoopTimerRef in_timer, void* user_data);

  private:
    EventLoopTimerRef timer;       //таймер
    TimerHandler      handler;     //обработчик
    void*             user_data;   //пользовательские данные
    EventRef          dummy_event; //событие, посылаемое при каждом вызове TimerProc
    common::Log       log;         //дебаг-лог
};

}

/*
    CarbonTimer
*/

CarbonTimer::CarbonTimer (size_t period_in_milliseconds, const TimerHandler& in_handler, void* in_user_data)
  : timer (0), handler (in_handler), user_data (in_user_data), log (LOG_NAME)
{
  try
  {
    check_event_manager_error (CreateEvent (0, 0, 0, 0, kEventAttributeNone, &dummy_event), "::CreateEvent", "Can't create dummy event");

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

  try
  {
    carbon_timer->handler (carbon_timer->user_data);

    EventQueueRef current_event_queue (GetCurrentEventQueue ());

    if (!IsEventInQueue (current_event_queue, carbon_timer->dummy_event))
      check_event_manager_error (PostEventToQueue (current_event_queue, carbon_timer->dummy_event, kEventPriorityLow),
                                 "::SendEventToEventTarget", "Can't dispatch dummy event");
  }
  catch (xtl::exception& exception)
  {
    carbon_timer->log.Printf ("Error at TimerProc: '%s'", exception.what ());
    printf ("Error at TimerProc: '%s'\n", exception.what ());
  }
  catch (...)
  {
    carbon_timer->log.Printf ("Error at TimerProc: unknown exception");
    printf ("Error at TimerProc: unkn exc\n");
  }
}

/*
    Создание / уничтожение таймера
*/

Platform::timer_t Platform::CreateTimer (size_t period_in_milliseconds, TimerHandler handler, void* user_data)
{
  return (timer_t)new CarbonTimer (period_in_milliseconds, handler, user_data);
}

void Platform::KillTimer (timer_t handle)
{
  if (!handle)
    throw xtl::make_null_argument_exception ("syslib::Platform::KillTimer", "handle");

  CarbonTimer* timer = (CarbonTimer*)handle;

  delete timer;
}
