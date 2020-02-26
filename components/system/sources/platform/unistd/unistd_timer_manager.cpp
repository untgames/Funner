#include <ctime>

#include <xtl/bind.h>
#include <xtl/common_exceptions.h>
#include <xtl/function.h>

#include <common/log.h>
#include <common/singleton.h>
#include <common/time.h>

#include <syslib/thread.h>

#include <shared/message_queue.h>
#include <shared/platform.h>

using namespace syslib;

/*
    Описание таймера
*/

struct syslib::timer_handle
{
  size_t       next_raise_time;        //время следующего срабатывания
  size_t       period_in_milliseconds; //период срабатывания
  TimerHandler handler;                //обработчик
  void*        user_data;              //пользовательские данные
  bool         need_delete;            //таймер требует удаления
  
  timer_handle (size_t in_period, TimerHandler in_handler, void* in_user_data)
    : next_raise_time (common::milliseconds () + in_period)
    , period_in_milliseconds (in_period)
    , handler (in_handler)
    , user_data (in_user_data)
    , need_delete (false)
  {
  }
};

namespace
{

static bool timer_manager_initialized = false;

/*
    Константы
*/

const char* LOG_NAME = "system.unistd"; //имя потока протоколирования

/*
    Менеджер таймеров
*/

class TimerManager: public MessageQueue::Handler
{
  public:
    typedef syslib::timer_t timer_t;
  
///Конструктор
    TimerManager ()
      : message_queue (*MessageQueueSingleton::Instance ())
      , log (LOG_NAME)
      , loop (true)
      , kill_lock (false)
      , new_timers_count (0)
      , thread (xtl::bind (&TimerManager::TimersRoutine, this))
    {
      if (!timer_manager_initialized)
      {
        timer_manager_initialized = true;
      }
      else
      {
        CloseTimersThread ();
        
        throw xtl::format_operation_exception ("syslib::TimerManager::TimerManager", "TimerManager can't be constructed more than one time");
      }
      
      try
      {        
        message_queue.RegisterHandler (*this);
      }
      catch (...)
      {
        CloseTimersThread ();
        throw;
      }
    }
    
///Деструктор
    ~TimerManager ()
    {
      try
      {
        CloseTimersThread ();
        
        message_queue.UnregisterHandler (*this);        
        
        UnregisterAllTimers ();

        timer_manager_initialized = false;
      }
      catch (...)
      {
        //подавление всех исключений
      }
    }
  
///Создание таймера
    timer_t CreateTimer (size_t period_in_milliseconds, TimerHandler handler, void* user_data)
    {
      try
      {
        stl::auto_ptr<timer_handle> timer;
        
        {
          Lock lock (mutex);

          timer = new timer_handle (period_in_milliseconds, handler, user_data);           
          
          timers.push_back (timer.get ());
          
          new_timers_count++;
        }
        
        condition.NotifyOne ();
        
        return timer.release ();
      }
      catch (xtl::exception& e)
      {
        e.touch ("syslib::TimerManager::CreateTimer");
        throw;
      }
    }
    
///Удаление таймера
    void KillTimer (timer_t timer)
    {
      try
      {
        if (!timer)
          throw xtl::make_null_argument_exception ("", "timer");
        
        {
          Lock lock (mutex);
        
          if (kill_lock)
          {
            timer->need_delete = true;
            return;
          }        
        
          timers.erase (stl::remove (timers.begin (), timers.end (), timer), timers.end ());
        
          delete timer;
        }
        
        condition.NotifyOne ();
      }
      catch (xtl::exception& e)
      {
        e.touch ("syslib::TimerManager::KillTimer");
        throw;
      }
    }

  private:
///Завершение нити обработки таймеров
    void CloseTimersThread ()
    {
      {
        Lock lock (mutex);
      
        loop = false;
      
        condition.NotifyOne ();
      }
      
      thread.Join ();      
    }
    
///Удаление всех таймеров
    void UnregisterAllTimers ()
    {      
      condition.NotifyOne ();
    }
    
///Сообщение таймера
    struct Message: public MessageQueue::Message
    {
      TimerManager& manager;
      timer_t       timer;
      
      Message (TimerManager& in_manager, timer_t in_timer)
        : manager (in_manager)
        , timer (in_timer) {}
      
      void Dispatch ()
      {
        manager.DispatchTimer (timer);
      }
    };
    
///Диспетчеризация вызова таймера
    void DispatchTimer (timer_t timer)
    {
      Lock lock (mutex);
      
      for (TimerList::iterator iter=timers.begin (), end=timers.end (); iter!=end; ++iter)
        if ((*iter) == timer)
        {
          if (timer->need_delete)
            return;
            
          timer->handler (timer->user_data);
          
          return;
        }
    }
  
///Нить обработки таймеров
    int TimersRoutine ()
    {
      while (loop)
      {
        Lock lock (mutex);
        
          //ожидание срабатывания таймеров
        
        while (loop)
        {
          size_t time      = common::milliseconds ();
          bool   raised    = false;
          size_t wait_time = ~0;
          
          for (TimerList::iterator iter=timers.begin (), end=timers.end (); iter!=end; ++iter)
          {
            if ((*iter)->next_raise_time <= time)
            {
              raised = true;
              break;
            }
            
            size_t delay = (*iter)->next_raise_time - time;
            
            if (delay < wait_time)
              wait_time = delay;
          }
          
          if (raised || !loop)
            break;
         
          if (new_timers_count)
          {
            new_timers_count--;
            continue;
          }
          
          static const float WAIT_DELAY_FACTOR = 0.8f;
            
          wait_time = size_t (wait_time * WAIT_DELAY_FACTOR);
          
//          printf ("[%08u // %08u]: wait in %u\n", int (clock () * 1000.0 / CLOCKS_PER_SEC), int (common::milliseconds ()), wait_time); fflush (stdout);          
          
          condition.TryWait (mutex, wait_time);
        }                
        
          //вызов обработчиков таймеров
          
        size_t time = common::milliseconds ();
        
        kill_lock = true;        
        
        for (TimerList::iterator iter=timers.begin (), end=timers.end (); iter!=end; ++iter)
        {
          timer_handle& timer = **iter;
          
          if (timer.next_raise_time > time)
            continue;
            
          try
          {
            message_queue.PushMessage (*this, xtl::intrusive_ptr<Message> (new Message (*this, &timer), false));
          }
          catch (std::exception& e)
          {
            log.Printf ("%s\n    at syslib::TimerManager::TimersRoutine", e.what ());
          }
          catch (...)
          {
            log.Printf ("Unknown exception\n    at syslib::TimerManager::TimersRoutine");            
          }
          
          timer.next_raise_time = time + timer.period_in_milliseconds;
        }
        
        kill_lock = false;
        
          //удаление таймеров
          
        for (TimerList::iterator iter=timers.begin (), end=timers.end (); iter!=end;)
        {
          if ((*iter)->need_delete) KillTimer (*iter);
          else                      ++iter;
        }
      }
      
      return 0;
    }
  
  private:
    typedef stl::vector<timer_t> TimerList;

  private:
    MessageQueue& message_queue;   //очередь собщений
    common::Log   log;             //поток протоколирования
    bool          loop;            //менеджер находится в состоянии обработки таймеров
    bool          kill_lock;       //таймеры заблокированы от удаления    
    Mutex         mutex;           //мьютекс работы с менеджером таймеров
    Condition     condition;       //событие появления нового таймера
    size_t        new_timers_count;
    Thread        thread;          //нить обработки таймеров
    TimerList     timers;          //список зарегистрированных таймеров
};

typedef common::Singleton<TimerManager> TimerManagerSingleton;

}

/*
    Создание / уничтожение таймера
*/

syslib::timer_t UnistdTimerManager::CreateTimer (size_t period_in_milliseconds, TimerHandler handler, void* user_data)
{
  try
  {
    TimerManager& manager = *TimerManagerSingleton::Instance ();

    return manager.CreateTimer (period_in_milliseconds, handler, user_data);
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::UnistdTimerManager::CreateTimer");
    throw;
  }
}

void UnistdTimerManager::KillTimer (timer_t handle)
{
  try
  {
    TimerManager& manager = *TimerManagerSingleton::Instance ();
    
    manager.KillTimer (handle);
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::UnistdTimerManager::KillTimer");
    throw;
  }
}
