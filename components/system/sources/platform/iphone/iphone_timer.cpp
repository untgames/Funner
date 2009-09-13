#include "shared.h"

using namespace syslib;

namespace
{

const char* LOG_NAME = "syslib.IPhoneTimer";

/*
    Описание реализации таймера
*/

class IPhoneTimer
{
  public:
    typedef Platform::TimerHandler TimerHandler;

    IPhoneTimer  (size_t period_in_milliseconds, const TimerHandler& handler, void* user_data);
    ~IPhoneTimer ();

  private:
    IPhoneTimer (const IPhoneTimer&); //no impl
    IPhoneTimer& operator = (const IPhoneTimer&); //no impl

    static void TimerProc (CFRunLoopTimerRef in_timer, void* user_data);

  private:
    CFRunLoopTimerRef timer;       //таймер
    TimerHandler      handler;     //обработчик
    void*             user_data;   //пользовательские данные
    common::Log       log;         //дебаг-лог
};

}

/*
    IPhoneTimer
*/

IPhoneTimer::IPhoneTimer (size_t period_in_milliseconds, const TimerHandler& in_handler, void* in_user_data)
  : timer (0), handler (in_handler), user_data (in_user_data), log (LOG_NAME)
{
  CFRunLoopTimerContext timer_context;

  memset (&timer_context, 0, sizeof (timer_context));

  timer_context.info    = this;

  double period_in_seconds = period_in_milliseconds / 1000.f;

  timer = CFRunLoopTimerCreate (0, CFAbsoluteTimeGetCurrent () + period_in_seconds, period_in_seconds, 0, 0, &IPhoneTimer::TimerProc, &timer_context);

  CFRunLoopAddTimer (CFRunLoopGetMain (), timer, kCFRunLoopCommonModes);
}

IPhoneTimer::~IPhoneTimer ()
{
  CFRunLoopTimerInvalidate (timer);

  CFRelease (timer);
}

void IPhoneTimer::TimerProc (CFRunLoopTimerRef in_timer, void* user_data)
{
  IPhoneTimer* iphone_timer = (IPhoneTimer*)user_data;

  try
  {
    iphone_timer->handler (iphone_timer->user_data);
  }
  catch (xtl::exception& exception)
  {
    iphone_timer->log.Printf ("Error at TimerProc: '%s'", exception.what ());
  }
  catch (...)
  {
    iphone_timer->log.Printf ("Error at TimerProc: unknown exception");
  }
}

/*
    Создание / уничтожение таймера
*/

Platform::timer_t Platform::CreateTimer (size_t period_in_milliseconds, TimerHandler handler, void* user_data)
{
  return (timer_t)new IPhoneTimer (period_in_milliseconds, handler, user_data);
}

void Platform::KillTimer (timer_t handle)
{
  if (!handle)
    throw xtl::make_null_argument_exception ("syslib::iPhonePlatform::KillTimer", "handle");

  IPhoneTimer* timer = (IPhoneTimer*)handle;

  delete timer;
}
