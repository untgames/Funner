#include "shared.h"

using namespace syslib;

#ifdef _MSC_VER
  #pragma warning (disable : 4355) //'this' : used in base member initializer list
#endif

/*
    Описание реализации таймера
*/

struct Timer::Impl
{
  Timer&            timer;      //таймер
  TickHandler       handler;    //обработчик срабатываний таймера
  size_t            period;     //период срабатываний (в милисекундах)
  Platform::timer_t sys_timer;  //дескриптор системного таймера
  size_t            start_time; //время старта таймера

  Impl  (Timer& in_timer, const TickHandler& in_handler);
  ~Impl ();

  void SetTimer  (size_t period);
  void KillTimer ();

  static void TimerHandler (void*);
};

/*
    Impl
*/

Timer::Impl::Impl (Timer& in_timer, const TickHandler& in_handler)
  : timer (in_timer), handler (in_handler), period (0), sys_timer (0)
  {}

Timer::Impl::~Impl ()
{
  KillTimer ();
}

void Timer::Impl::SetTimer (size_t in_period)
{
  if (sys_timer && period == in_period)
    return;

  KillTimer ();

  period = in_period;

  sys_timer  = Platform::CreateTimer (period, &TimerHandler, this);
  start_time = common::milliseconds ();
}

void Timer::Impl::KillTimer ()
{
  if (!sys_timer)
    return;

  Platform::KillTimer (sys_timer);

  sys_timer = 0;
}

void Timer::Impl::TimerHandler (void* user_data)
{
  Impl* impl = (Impl*)user_data;

  impl->handler (impl->timer);
}

/*
    Конструктор / деструктор
*/

Timer::Timer (const TickHandler& in_handler, size_t period_in_milliseconds, TimerState initial_state)
  : impl (new Impl (*this, in_handler))
{
  SetPeriod (period_in_milliseconds);
  SetState (initial_state);
}

Timer::~Timer ()
{
  delete impl;
}

/*
    Обработчик
*/

const Timer::TickHandler& Timer::Handler () const
{
  return impl->handler;
}

/*
    Период
*/

size_t Timer::Period () const
{
  return impl->period;
}

void Timer::SetPeriod (size_t period_in_milliseconds)
{
  impl->SetTimer (period_in_milliseconds);
}

/*
    Приостановка / возобновление работы таймера
*/

void Timer::SetState (TimerState state)
{
  switch (state)
  {
    case TimerState_Running: impl->SetTimer (impl->period); break;
    case TimerState_Paused:  impl->KillTimer (); break;
    default:                 throw xtl::make_argument_exception ("syslib::Timer::SetState", "state", state); break;
  }
}

TimerState Timer::State () const
{
  return impl->sys_timer ? TimerState_Running : TimerState_Paused;
}

/*
    Количество милисекунд, прошедшее с момента старта таймера
*/

size_t Timer::ElapsedMilliseconds () const
{
  return common::milliseconds () - impl->start_time;
}
