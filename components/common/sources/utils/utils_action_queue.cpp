#include <stl/hash_map>

#include <xtl/bind.h>
#include <xtl/common_exceptions.h>
#include <xtl/function.h>
#include <xtl/intrusive_ptr.h>
#include <xtl/lock_ptr.h>
#include <xtl/reference_counter.h>
#include <xtl/signal.h>
#include <xtl/trackable_ptr.h>

#include <common/action_queue.h>
#include <common/component.h>
#include <common/lockable.h>
#include <common/singleton.h>

#include <platform/platform.h>

#ifdef _MSC_VER
  #pragma warning (disable : 4355) //'this' : used in base member initializer list
#endif

using namespace common;

namespace common
{

/*
    Константы
*/

const char* ACTION_QUEUE_LISTENERS_COMPONENT_MASK = "common.action_queue.*"; //маска имён компонентов слушателй событий очереди действий

/*
    Реализация действия
*/

struct ActionImpl;

typedef xtl::lock_ptr<ActionImpl, xtl::intrusive_ptr<ActionImpl> > ActionLock;

struct ActionImpl: public xtl::reference_counter, public Lockable
{
  ActionQueue::ActionHandler     action_handler;  //обработчик выполнения действия
  Action::WaitCompleteHandler    wait_handler;    //обработчик ожидания выполнения операции
  ActionThread                   thread_type;     //тип нити
  Timer                          timer;           //таймер, связанный с действием
  ActionQueue::time_t            next_time;       //время следующего выполнения действия
  ActionQueue::time_t            period;          //период выполнения действия
  size_t                         thread_id;       //идентификатор нити, в которой создано действие
  bool                           is_periodic;     //является ли действие периодическим
  bool                           is_completed;    //завершено ли действие
  bool                           is_canceled;     //действие отменено

  ActionImpl (const ActionQueue::ActionHandler& in_handler, ActionThread in_thread_type, bool in_is_periodic, Timer& in_timer, const ActionQueue::time_t& delay, const ActionQueue::time_t& in_period, const Action::WaitCompleteHandler& in_wait_handler)
    : action_handler (in_handler)
    , wait_handler (in_wait_handler)
    , thread_type (in_thread_type)
    , timer (in_timer)
    , next_time (timer.Time () + delay)    
    , period (in_period)
    , thread_id (Platform::GetCurrentThreadId ())
    , is_periodic (in_is_periodic)
    , is_completed (false)
    , is_canceled (false)
  {
    //next time and period should have same denominator. In other case denominator will grow on each Action re-queue due to rational arithmetics
    ActionQueue::time_t::int_type next_time_denominator = next_time.denominator (),
                                  period_denominator    = period.denominator ();

    if (next_time_denominator != period_denominator)
    {
      ActionQueue::time_t::int_type next_time_period_denominator;

      if (next_time_denominator % period_denominator == 0)
      {
        next_time_period_denominator = next_time_denominator;
      }
      else if (period_denominator % next_time_denominator == 0)
      {
        next_time_period_denominator = period_denominator;
      }
      else
      {
        next_time_period_denominator = next_time_denominator * period_denominator;
      }

      if (next_time_denominator != next_time_period_denominator)
      {
        next_time.assign (next_time.numerator () * (next_time_period_denominator / next_time_denominator), next_time_period_denominator);
      }

      if (period_denominator != next_time_period_denominator)
      {
        period.assign (period.numerator () * (next_time_period_denominator / period_denominator), next_time_period_denominator);
      }
    }
  }
  
  Action GetWrapper () { return Action (this); }
  
  static xtl::intrusive_ptr<ActionImpl> GetImpl (Action& action) { return action.impl; }
};

typedef xtl::intrusive_ptr<ActionImpl> ActionPtr;

}

namespace
{

/*
    Действие с обратным вызовом по завершению
*/

class ActionWithCallback
{
  public:
    ActionWithCallback (const ActionQueue::ActionHandler& in_action, const ActionQueue::CallbackHandler& in_complete_handler)
      : action (in_action)
      , complete_callback (in_complete_handler)
    {
    }
    
    void operator () (Action& state) const
    {
      action (state);
      complete_callback ();
    }

  private:
    ActionQueue::ActionHandler   action;
    ActionQueue::CallbackHandler complete_callback;
};

/*
    Очередь нити
*/

class ThreadActionQueue: public xtl::reference_counter
{
  public:
///Конструктор
    ThreadActionQueue ()
    {
      actions_count = 0;
    }
  
///Проверка на пустоту
    bool IsEmpty ()
    {
      return actions.empty ();
    }
    
///Количество действий в очереди
    size_t Size ()
    {
      return actions_count;
    }
  
///Добавление действия
    void PushAction (const ActionPtr& action)
    {
      if (!action)
        return;
        
      actions.push_back (action);
      
      actions_count++;
    }    
    
///Получение действия
    ActionPtr PopAction ()
    {
      for (ActionList::iterator iter=actions.begin (); iter!=actions.end ();)
      {
        ActionLock action (iter->get ());
        
        if (action->is_canceled || action->is_completed)
        {
          ActionList::iterator next = iter;

          ++next;

          actions.erase (iter);
          
          actions_count--;

          iter = next;

          continue;
        }

        if (action->next_time > action->timer.Time ())
        {
          ++iter;
          continue;
        }                

        if (action->is_periodic)
        {
          actions.splice (actions.end (), actions, iter);          
          
          action->next_time += action->period;

          return action.get ();
        }
          
        actions.erase (iter);
        
        actions_count--;

        return action.get ();
      }

      return ActionPtr ();
    }

  private:
    typedef stl::list<ActionPtr> ActionList;

  private:
    size_t     actions_count;
    ActionList actions;
};

typedef xtl::intrusive_ptr<ThreadActionQueue> ThreadActionQueuePtr;

/*
    Реализация очереди действий
*/

class ActionQueueImpl
{
  public:
    typedef ActionQueue::time_t          time_t;
    typedef ActionQueue::ActionHandler   ActionHandler;
    typedef ActionQueue::CallbackHandler CallbackHandler;
    
///Конструктор
    ActionQueueImpl ()
      : default_wait_handler (xtl::bind (&ActionQueueImpl::DefaultWaitHandler, _1, _2))
    {
      default_timer.Start ();
    }

///Добавление действия в очередь
    Action PushAction (const ActionHandler& action_handler, ActionThread thread, bool is_periodic, const time_t& delay, const time_t& period, Timer& timer)
    {
      try
      {
        switch (thread)
        {
          case ActionThread_Current:
          case ActionThread_Main:
          case ActionThread_Background:
            break;
          default:
            throw xtl::make_argument_exception ("", "thread", thread);
        }

        ActionPtr action (new ActionImpl (action_handler, thread, is_periodic, timer, delay, period, default_wait_handler), false);

        ThreadActionQueue& queue = GetQueue (thread);
        
        return PushAction (action, queue);
      }
      catch (xtl::exception& e)
      {
        e.touch ("common::ActionQueue::PushAction(const ActionHandler&,ActionThread,bool,time_t,time_t,Timer&)");
        throw;
      }
    }
    
    Action PushAction (const ActionHandler& action_handler, size_t thread_id, bool is_periodic, const time_t& delay, const time_t& period, Timer& timer)
    {
      try
      {
        ThreadActionQueue& queue = GetQueue (ActionThread_Current, thread_id);        
        
        ActionPtr action (new ActionImpl (action_handler, ActionThread_Current, is_periodic, timer, delay, period, default_wait_handler), false);
        
        action->thread_id = thread_id;
        
        return PushAction (action, queue);
      }
      catch (xtl::exception& e)
      {
        e.touch ("common::ActionQueue::PushAction(const ActionHandler&,size_t,bool,time_t,time_t,Timer&)");
        throw;
      }
    }    

///Размер очереди
    size_t ActionsCount (ActionThread thread)
    {
      try
      {
        switch (thread)
        {
          case ActionThread_Main:
            return main_thread_queue.Size ();
          case ActionThread_Background:
            return background_queue.Size ();
          case ActionThread_Current:
            return ActionsCount (Platform::GetCurrentThreadId ());
          default:
            throw xtl::make_argument_exception ("", "thread", thread);
        }
      }
      catch (xtl::exception& e)
      {
        e.touch ("common::ActionQueue::ActionsCount(ActionThread)");
        throw;
      }
    }
    
    size_t ActionsCount (size_t thread_id)
    {
      try
      {
        ThreadActionQueueMap::iterator iter = thread_queues.find (thread_id);

        if (iter == thread_queues.end ())
          return 0;

        ThreadActionQueuePtr queue = iter->second;

        return queue->Size ();
      }
      catch (xtl::exception& e)
      {
        e.touch ("common::ActionQueue::ActionsCount(size_t)");
        throw;
      }
    }

///Извлечение действия из очереди
    Action PopAction (ActionThread thread)
    {
      try
      {
        ThreadActionQueue& queue = GetQueue (thread);

        ActionPtr action = queue.PopAction ();
        
        if (!action)
          return Action ();
          
        Action result = action->GetWrapper ();
          
        try
        {
          signals [ActionQueueEvent_OnPopAction] (thread, result);
        }
        catch (...)
        {
          //подавление всех исключений
        }        

        return result;
      }
      catch (xtl::exception& e)
      {
        e.touch ("common::ActionQueue::PopAction");
        throw;
      }      
    }

///Очистка вспомогательных структур для текущей нити
    void CleanupCurrentThread ()
    {
      try
      {
        Platform::threadid_t thread_id = Platform::GetCurrentThreadId ();

        ThreadActionQueueMap::iterator iter = thread_queues.find (thread_id);

        if (iter == thread_queues.end ())
          return;

        ThreadActionQueuePtr queue = iter->second;

        if (!queue->IsEmpty ())
          throw xtl::format_operation_exception ("", "Can't cleanup current thread queue. Queue is not empty");

        thread_queues.erase (iter);
      }
      catch (xtl::exception& e)
      {
        e.touch ("common::ActionQueue::CleanupCurrentThread");
        throw;
      }
    }
    
///Регистрация обработчиков событий
    xtl::connection RegisterEventHandler (ActionQueueEvent event, const ActionQueue::EventHandler& handler)
    {
      switch (event)
      {
        case ActionQueueEvent_OnPushAction:
        case ActionQueueEvent_OnPopAction:
          break;
        default:
          throw xtl::make_argument_exception ("common::ActionQueue::RegisterEventHandler", "event", event);
      }

      return signals [event].connect (handler);
    }

///Получение таймера по умолчанию
    Timer& DefaultTimer () { return default_timer; }    

  private:
///Помещение действия в очередь
    Action PushAction (const ActionPtr& action, ThreadActionQueue& queue)
    {
      try
      {
        queue.PushAction (action);

        Action result = action->GetWrapper ();

        try
        {
          static common::ComponentLoader loader (ACTION_QUEUE_LISTENERS_COMPONENT_MASK);
          
          signals [ActionQueueEvent_OnPushAction] (action->thread_type, result);
        }
        catch (...)
        {
          //подавление всех исключений
        }

        return result;
      }
      catch (xtl::exception& e)
      {
        e.touch ("common::ActionQueue::PushAction");
        throw;
      }
    }  
  
///Получение очереди нити
    ThreadActionQueue& GetQueue (ActionThread thread, size_t thread_id = Platform::GetCurrentThreadId ())
    {
      switch (thread)
      {
        case ActionThread_Main:
          return main_thread_queue;
        case ActionThread_Background:
          return background_queue;
        case ActionThread_Current:
        {
          ThreadActionQueueMap::iterator iter = thread_queues.find (thread_id);
          
          if (iter != thread_queues.end ())
            return *iter->second;
            
          ThreadActionQueuePtr queue (new ThreadActionQueue, false);

          thread_queues.insert_pair (thread_id, queue);

          return *queue;
        }
        default:
          throw xtl::make_argument_exception ("common::ActionQueueImpl::GetQueue", "thread", thread);
      }
    }
    
///Функция ожидания по умолчанию
    static bool DefaultWaitHandler (Action& action, size_t milliseconds)
    {
      size_t       end_time = common::milliseconds () + milliseconds;
      bool         infinite = milliseconds == ~0u;
      ActionThread thread   = action.ThreadType ();
      
      while (!action.IsCompleted () && !action.IsCanceled () && (infinite || common::milliseconds () < end_time))
      {
        Action performed_action = ActionQueue::PopAction (thread);
        
        if (performed_action.IsEmpty () || performed_action.IsCanceled () || performed_action.IsCompleted ())
          continue;

        performed_action.Perform ();
      }

      return action.IsCompleted ();
    }

  private:
    typedef stl::hash_map<Platform::threadid_t, ThreadActionQueuePtr> ThreadActionQueueMap;
    typedef xtl::signal<void (ActionThread, Action&)>                 Signal;

  private:
    Timer                       default_timer;
    Action::WaitCompleteHandler default_wait_handler;
    ThreadActionQueue           main_thread_queue;
    ThreadActionQueue           background_queue;
    ThreadActionQueueMap        thread_queues;
    Signal                      signals [ActionQueueEvent_Num];
};

typedef common::Singleton<ActionQueueImpl> ActionQueueSingleton;

}

/*
    Врапперы
*/

Action ActionQueue::PushAction (const ActionHandler& action, ActionThread thread, const time_t& delay)
{
  ActionQueueSingleton::Instance queue;

  return queue->PushAction (action, thread, false, delay, ActionQueue::time_t (0), queue->DefaultTimer ());
}

Action ActionQueue::PushAction (const ActionHandler& action, ActionThread thread, const time_t& delay, const time_t& period)
{
  ActionQueueSingleton::Instance queue;

  return queue->PushAction (action, thread, true, delay, period, queue->DefaultTimer ());
}

Action ActionQueue::PushAction (const ActionHandler& action, ActionThread thread, const time_t& delay, Timer& timer)
{
  ActionQueueSingleton::Instance queue;

  return queue->PushAction (action, thread, false, delay, ActionQueue::time_t (0), timer);
}

Action ActionQueue::PushAction (const ActionHandler& action, ActionThread thread, const time_t& delay, const time_t& period, Timer& timer)
{
  ActionQueueSingleton::Instance queue;

  return queue->PushAction (action, thread, true, delay, period, timer);
}

Action ActionQueue::PushAction (const ActionHandler& action, const CallbackHandler& complete_callback, ActionThread thread, const time_t& delay)
{
  return PushAction (ActionWithCallback (action, complete_callback), thread, delay);
}

Action ActionQueue::PushAction (const ActionHandler& action, const CallbackHandler& complete_callback, ActionThread thread, const time_t& delay, Timer& timer)
{
  return PushAction (ActionWithCallback (action, complete_callback), thread, delay, timer);
}

Action ActionQueue::PushAction (const ActionHandler& action, size_t thread_id, const time_t& delay)
{
  ActionQueueSingleton::Instance queue;

  return queue->PushAction (action, thread_id, false, delay, ActionQueue::time_t (0), queue->DefaultTimer ());
}

Action ActionQueue::PushAction (const ActionHandler& action, size_t thread_id, const time_t& delay, const time_t& period)
{
  ActionQueueSingleton::Instance queue;

  return queue->PushAction (action, thread_id, true, delay, period, queue->DefaultTimer ());
}

Action ActionQueue::PushAction (const ActionHandler& action, size_t thread_id, const time_t& delay, Timer& timer)
{
  ActionQueueSingleton::Instance queue;

  return queue->PushAction (action, thread_id, false, delay, ActionQueue::time_t (0), timer);
}

Action ActionQueue::PushAction (const ActionHandler& action, size_t thread_id, const time_t& delay, const time_t& period, Timer& timer)
{
  ActionQueueSingleton::Instance queue;

  return queue->PushAction (action, thread_id, true, delay, period, timer);
}

Action ActionQueue::PushAction (const ActionHandler& action, const CallbackHandler& complete_callback, size_t thread_id, const time_t& delay)
{
  return PushAction (ActionWithCallback (action, complete_callback), thread_id, delay);
}

Action ActionQueue::PushAction (const ActionHandler& action, const CallbackHandler& complete_callback, size_t thread_id, const time_t& delay, Timer& timer)
{
  return PushAction (ActionWithCallback (action, complete_callback), thread_id, delay, timer);
}

size_t ActionQueue::ActionsCount (ActionThread thread)
{
  return ActionQueueSingleton::Instance ()->ActionsCount (thread);
}

size_t ActionQueue::ActionsCount (size_t thread_id)
{
  return ActionQueueSingleton::Instance ()->ActionsCount (thread_id);
}

Action ActionQueue::PopAction (ActionThread thread)
{
  return ActionQueueSingleton::Instance ()->PopAction (thread);
}

xtl::connection ActionQueue::RegisterEventHandler (ActionQueueEvent event, const EventHandler& handler)
{
  return ActionQueueSingleton::Instance ()->RegisterEventHandler (event, handler);
}

void ActionQueue::CleanupCurrentThread ()
{
  ActionQueueSingleton::Instance ()->CleanupCurrentThread ();
}

/*
    Действие
*/

Action::Action ()
  : impl (0)
{
}

Action::Action (ActionImpl* in_impl)
  : impl (in_impl)
{
  if (impl)
    addref (impl);
}

Action::Action (const Action& action)
  : impl (action.impl)
{
  ActionLock locked_impl (impl);

  if (impl)
    addref (impl);
}

Action::~Action ()
{
  ActionLock locked_impl (impl);

  if (impl)
    release (impl);
}

Action& Action::operator = (const Action& action)
{
  Action (action).Swap (*this);

  return *this;
}

size_t Action::CreatorThreadId () const
{
  return impl ? impl->thread_id : 0;
}

ActionThread Action::ThreadType () const
{
  return impl ? impl->thread_type : ActionThread_Current;
}

bool Action::IsEmpty () const
{
  return impl == 0;
}

bool Action::IsCompleted () const
{
  ActionLock locked_impl (impl);

  return impl ? impl->is_completed : true;
}

bool Action::IsCanceled () const
{
  ActionLock locked_impl (impl);

  return impl ? locked_impl->is_canceled : false;
}

bool Action::Wait (size_t milliseconds)
{
  try
  {
    ActionLock locked_impl (impl);
    
    if (!impl)
      return true;

    return impl->wait_handler (*this, milliseconds);
  }
  catch (xtl::exception& e)
  {
    e.touch ("common::Action::Wait");
    throw;
  }
}

void Action::Perform ()
{
  try
  {
    xtl::intrusive_ptr<ActionImpl> impl_holder;

    ActionQueue::ActionHandler handler;    

    {
      ActionLock locked_impl (impl);

      if (!impl)
        return;

      if (impl->is_completed)
        throw xtl::format_operation_exception ("", "Action already completed");

      if (impl->is_canceled)
        throw xtl::format_operation_exception ("", "Action already canceled");

      handler     = impl->action_handler;
      impl_holder = impl;
    }

    handler (*this);

    ActionLock locked_impl (impl);
    
    if (!impl->is_periodic)
      impl->is_completed = true;
  }
  catch (xtl::exception& e)
  {
    e.touch ("common::Action::Perform");
    throw;
  }
}

void Action::Cancel ()
{
  ActionLock locked_impl (impl);
  
  if (!impl)
    return;

  impl->is_canceled = true;
}

void Action::SetWaitHandler (const WaitCompleteHandler& handler)
{
  ActionLock locked_impl (impl);
  
  if (!impl)
    throw xtl::format_operation_exception ("common::Action::SetWaitHandler", "Can't set wait handler for empty action");

  impl->wait_handler = handler;
}

const Action::WaitCompleteHandler& Action::WaitHandler () const
{
  ActionLock locked_impl (impl);

  return impl->wait_handler;
}

void Action::Swap (Action& action)
{
  if (impl < action.impl)
  {
    ActionLock locked_impl1 (impl);
    ActionLock locked_impl2 (action.impl);
    
    stl::swap (impl, action.impl);
  }
  else
  {
    ActionLock locked_impl1 (action.impl);
    ActionLock locked_impl2 (impl);

    stl::swap (impl, action.impl);    
  }
}

namespace common
{

void swap (Action& action1, Action& action2)
{
  action1.Swap (action2);
}

/*
    Создание диспетчера функции обратного вызова в указанной нити
*/

namespace
{

struct ThreadCallbackWrapper
{
  ActionQueue::ActionHandler callback_handler;
  ActionThread               thread_type;
  size_t                     thread_id;
  
  ThreadCallbackWrapper (const ActionQueue::CallbackHandler& in_callback_handler, ActionThread in_thread_type)
    : callback_handler (xtl::bind (in_callback_handler))
    , thread_type (in_thread_type)
    , thread_id (Platform::GetCurrentThreadId ())
  {
  }
  
  void operator () ()
  {
    ActionQueueSingleton::Instance queue;
    
    switch (thread_type)
    {
      case ActionThread_Main:
      case ActionThread_Background:        
        queue->PushAction (callback_handler, thread_type, false, ActionQueue::time_t (0), ActionQueue::time_t (0), queue->DefaultTimer ());
        break;
      case ActionThread_Current:
        queue->PushAction (callback_handler, thread_id, false, ActionQueue::time_t (0), ActionQueue::time_t (0), queue->DefaultTimer ());
        break;
      default:
        throw xtl::format_operation_exception ("common::ThreadCallbackWrapper::operator()", "Bad ActionThread %d", thread_type);
    }
  }
};

}

ActionQueue::CallbackHandler make_callback_wrapper (ActionThread thread, const ActionQueue::CallbackHandler& handler)
{
  try
  {
    switch (thread)
    {
      case ActionThread_Current:
      case ActionThread_Main:
      case ActionThread_Background:
        break;
      default:
        throw xtl::make_argument_exception ("", "thread", thread);
    }
    
    return ThreadCallbackWrapper (handler, thread);
  }
  catch (xtl::exception& e)
  {
    e.touch ("common::make_callback_wrapper");
    throw;
  }
}

}
