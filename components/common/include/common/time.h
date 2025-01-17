#ifndef COMMONLIB_TIME_HEADER
#define COMMONLIB_TIME_HEADER

#include <cstddef>

#include <xtl/functional_fwd>
#include <xtl/rational.h>

namespace common
{

//implementation forward
struct TimerImpl;

//получение времени в миллисекундах от первого вызова milliseconds
size_t milliseconds ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Таймер с минимальной погрешностью
///////////////////////////////////////////////////////////////////////////////////////////////////
class Timer
{
  public:
    typedef long long int               timeint_t;
    typedef xtl::rational<timeint_t>    time_t;
    typedef xtl::function<timeint_t ()> TimeSource;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструкторы / деструктор / присваивание
///////////////////////////////////////////////////////////////////////////////////////////////////
    Timer  (bool is_auto_update = true);
    Timer  (const TimeSource& source, timeint_t precision, bool is_auto_update);
    Timer  (const Timer& source, bool is_auto_update);
    Timer  (const Timer&);
    ~Timer ();

    Timer& operator = (const Timer&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Установка значения таймера
///////////////////////////////////////////////////////////////////////////////////////////////////
    void          SetTime (const time_t& time);
    const time_t& Time    () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Обновление значения времени
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Update ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Управление авто-обновляемыми таймерами
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetAutoUpdate (bool state);
    bool IsAutoUpdate  () const;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Управление таймером
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Start ();
    void Stop  ();
    void Reset ();

    bool IsStarted () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Обмен
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Swap (Timer&);

  private:
    TimerImpl* impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Обмен
///////////////////////////////////////////////////////////////////////////////////////////////////
void swap (Timer&, Timer&);

}

#endif
