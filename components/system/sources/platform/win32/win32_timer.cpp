#include "shared.h"

using namespace syslib;
using namespace common;

/*
    Описание реализации таймера
*/

namespace
{

class Win32Timer
{
  public:
    typedef Platform::TimerHandler TimerHandler;
  
    Win32Timer  (size_t period_in_milliseconds, const TimerHandler& handler, void* user_data);
    ~Win32Timer ();

  private:
    Win32Timer (const Win32Timer&); //no impl
    Win32Timer& operator = (const Win32Timer&); //no impl
    
    static VOID CALLBACK TimerProc (HWND, UINT, UINT_PTR, DWORD);
    
    typedef stl::hash_map<UINT_PTR, Win32Timer*> TimerMap;
    typedef common::Singleton<TimerMap>          TimerMapSingleton;    

  private:  
    UINT_PTR     timer;     //дескриптор таймера
    TimerHandler handler;   //обработчик
    void*        user_data; //пользовательские данные
};

}

/*
    Win32Timer
*/

Win32Timer::Win32Timer (size_t period_in_milliseconds, const TimerHandler& in_handler, void* in_user_data)
  : handler (in_handler), user_data (in_user_data)
{
  try
  {
    SetLastError (0);

    timer = SetTimer (0, 0, period_in_milliseconds, &TimerProc);

    if (!timer)
      raise_error ("::SetTimer");

    try
    {
      TimerMapSingleton::Instance () [timer] = this;
    }
    catch (...)
    {
      KillTimer (0, timer);
      SetLastError (0);
      throw;
    }
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::Win32Timer::Win32Timer");
    throw;
  }
}

Win32Timer::~Win32Timer ()
{
  try
  {
    TimerMapSingleton::Instance ().erase (timer);
  }
  catch (...)
  {
    //подавляем все исключения
  }

  KillTimer (0, timer);
  SetLastError (0);
}

VOID CALLBACK Win32Timer::TimerProc (HWND, UINT, UINT_PTR timer, DWORD)
{
  try
  {
    TimerMap&          timer_map = TimerMapSingleton::Instance ();
    TimerMap::iterator iter      = timer_map.find (timer);
    
    if (iter == timer_map.end ())
      return;
    
    Win32Timer* timer = iter->second;
    
    timer->handler (timer->user_data);
  }
  catch (...)
  {
    //подавляем все исключения
    //в будущем желательно протоколировать информацию об исключениях в Platform::DebugLog
  }
}

/*
    Приостановка выполнения приложения на miliseconds милисекунд
*/

void Platform::Sleep (size_t miliseconds)
{
  SetLastError (0);
  ::Sleep (miliseconds);
}

/*
    Создание / уничтожение таймера
*/

Platform::timer_t Platform::CreateTimer (size_t period_in_milliseconds, TimerHandler handler, void* user_data)
{
  return (timer_t)new Win32Timer (period_in_milliseconds, handler, user_data);
}

void Platform::KillTimer (timer_t handle)
{
  if (!handle)
    throw xtl::make_null_argument_exception ("syslib::Platform::KillTimer", "handle");

  Win32Timer* timer = (Win32Timer*)handle;

  delete timer;
}
