#include <xtl/function.h>
#include <xtl/intrusive_ptr.h>
#include <xtl/reference_counter.h>

#include <common/singleton.h>
#include <common/time.h>

#include <platform/platform.h>

using namespace common;

/*
    Реализация таймер с минимальной погрешностью
*/

namespace
{

size_t start_milliseconds_time = Platform::GetMilliseconds ();

}

namespace common
{

struct TimerImpl: public xtl::reference_counter
{
  typedef Timer::TimeSource TimeSource;
  typedef Timer::time_t     time_t;
  typedef Timer::timeint_t  timeint_t;

  TimeSource source;          //источник получения времени
  bool       auto_update;     //обновляется ли время автоматически
  time_t     time;            //текущее время
  timeint_t  additional_time; //время, прибавляемое к текущему времени, для реализации SetTime
  timeint_t  start_time;      //стартовое время
  timeint_t  current_time;    //текущее время
  bool       started;         //включен ли таймер
  
///Конструктор
  TimerImpl (const TimeSource& in_source, timeint_t precision, bool in_auto_update)
    : source (in_source)
    , auto_update (in_auto_update)
    , time (0, precision)
    , additional_time (0)
    , start_time (0)
    , current_time (0)
    , started (false)
  {
  }
  
///Обновление
  void Update ()
  {
    if (!started)
      return;
      
    current_time = ComputeTimeInteger ();

    time.assign (current_time + additional_time, time.denominator ());
  }
  
///Получение целочисленного времени
  timeint_t GetCurrentTimeInteger ()
  {
    if (auto_update)
      Update ();
      
    return current_time + additional_time;
  }
  
///Расчёт интервала времени
  timeint_t ComputeTimeInteger ()
  {
    return started ? source () - start_time : start_time;
  }
};

}

namespace
{

/*
    Источник отсчёта времени по умолчанию
*/

struct DefaultTimeSource
{
  Timer::TimeSource source;
  
  DefaultTimeSource () : source (&common::milliseconds) { }
  
  static Timer::TimeSource& Instance ()
  {
    return common::Singleton<DefaultTimeSource>::Instance ()->source;
  }
};

/*
    Обёртка над таймером для наследования управления временем
*/

struct ParentTimeSource
{
  xtl::intrusive_ptr<TimerImpl> impl;
  
  ParentTimeSource (TimerImpl* in_impl) : impl (in_impl) {}

  Timer::timeint_t operator () () const
  {
    return impl->GetCurrentTimeInteger ();
  }
};

}

/*
    Конструкторы / деструктор / присваивание
*/

Timer::Timer (bool is_auto_update)
  : impl (new TimerImpl (DefaultTimeSource::Instance (), 1000, is_auto_update))
{
}

Timer::Timer (const TimeSource& source, timeint_t precision, bool is_auto_update)
  : impl (new TimerImpl (source, precision, is_auto_update))
{
}

Timer::Timer (const Timer& source, bool is_auto_update)
  : impl (new TimerImpl (ParentTimeSource (source.impl), source.impl->time.denominator (), is_auto_update))
{
}

Timer::Timer (const Timer& source)
  : impl (source.impl)
{
  addref (impl);
}

Timer::~Timer ()
{
  release (impl);
}

Timer& Timer::operator = (const Timer& source)
{
  Timer (source).Swap (*this);
  
  return *this;
}

/*
    Установка значения таймера
*/

void Timer::SetTime (const time_t& time)
{
  impl->current_time    = impl->started ? impl->source () : 0;
  impl->start_time      = impl->current_time;
  impl->additional_time = time.numerator () * impl->time.denominator () / time.denominator ();

  impl->time.assign (impl->additional_time, impl->time.denominator ());
}

const Timer::time_t& Timer::Time () const
{
  if (impl->auto_update)
    impl->Update ();

  return impl->time;
}

/*
    Обновление значения времени
*/

void Timer::Update ()
{
  impl->Update ();
}

/*
    Управление авто-обновляемыми таймерами
*/

void Timer::SetAutoUpdate (bool state)
{
  impl->auto_update = state;
}

bool Timer::IsAutoUpdate () const
{
  return impl->auto_update;
}

/*
    Управление таймером
*/

void Timer::Start ()
{
  if (impl->started)
    return;

  impl->start_time = impl->source () - impl->start_time;    
  impl->started    = true;
}

void Timer::Stop ()
{
  if (!impl->started)
    return;

  Update ();

  impl->start_time = impl->source () - impl->start_time;
  impl->started    = false;
}

void Timer::Reset ()
{
  impl->started         = false;
  impl->time            = time_t (0, impl->time.denominator ());
  impl->additional_time = 0;
  impl->start_time      = 0;
  impl->current_time    = 0;
}

bool Timer::IsStarted () const
{
  return impl->started;
}

/*
    Обмен
*/

void Timer::Swap (Timer& timer)
{
  TimerImpl* tmp = impl;
  impl           = timer.impl;
  timer.impl     = tmp;
}

namespace common
{

//Обмен двух таймеров
void swap (Timer& timer1, Timer& timer2)
{
  timer1.Swap (timer2);
}

//Получение времени в миллисекундах от первого вызова milliseconds
size_t milliseconds ()
{  
  return Platform::GetMilliseconds () - start_milliseconds_time;
}

}
