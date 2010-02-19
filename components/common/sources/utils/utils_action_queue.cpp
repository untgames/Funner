#include <stl/hash_map>

#include <xtl/bind.h>
#include <xtl/common_exceptions.h>
#include <xtl/function.h>
#include <xtl/intrusive_ptr.h>
#include <xtl/lock_ptr.h>
#include <xtl/reference_counter.h>
#include <xtl/signal.h>

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

struct ActionImpl: public xtl::reference_counter, public Lockable
{
  ActionQueue::ActionHandler  action_handler; //обработчик выполнения действия
  Action::WaitCompleteHandler wait_handler;   //обработчик ожидания выполнения операции  
  Timer                       timer;          //таймер, связанный с действием
  ActionQueue::time_t         next_time;      //время следующего выполнения действия
  ActionQueue::time_t         period;         //период выполнения действия
  bool                        is_periodic;    //является ли действие периодическим
  bool                        is_completed;   //завершено ли действие
  bool                        is_canceled;    //действие отменено

  ActionImpl (const ActionQueue::ActionHandler& in_handler, bool in_is_periodic, Timer& in_timer, ActionQueue::time_t delay, ActionQueue::time_t in_period, const Action::WaitCompleteHandler& in_wait_handler)
    : action_handler (in_handler)
    , wait_handler (in_wait_handler)
    , timer (in_timer)
    , next_time (timer.Time () + delay)
    , period (in_period)
    , is_periodic (in_is_periodic)
    , is_completed (false)
    , is_canceled (false)
  {
  }
  
  Action GetWrapper () { return Action (this); }
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

typedef xtl::lock_ptr<ActionImpl, xtl::intrusive_ptr<ActionImpl> > ActionLock;

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
        ActionLock action (actions.front ().get ());
        
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

/*
    Реализация очереди действий
*/

class ActionQueueImpl
{
  public:
    typedef ActionQueue::time_t        time_t;
    typedef ActionQueue::ActionHandler ActionHandler;
    
///Конструктор
    ActionQueueImpl ()
      : default_wait_handler (xtl::bind (&ActionQueueImpl::DefaultWaitHandler, _1, _2))
      , default_action (new ActionImpl (ActionHandler (), false, default_timer, 0.0, 0.0, default_wait_handler), false)
    {
      default_timer.Start ();
    }

///Добавление действия в очередь
    Action PushAction (const ActionHandler& action_handler, ActionThread thread, bool is_periodic, time_t delay, time_t period, Timer& timer)
    {
      try
      {
        ActionPtr action (new ActionImpl (action_handler, is_periodic, timer, delay, period, default_wait_handler), false);

        ThreadActionQueue& queue = GetQueue (thread);

        queue.PushAction (action);

        Action result = action->GetWrapper ();

        try
        {
          static common::ComponentLoader loader (ACTION_QUEUE_LISTENERS_COMPONENT_MASK);
          
          signals [ActionQueueEvent_OnPushAction] (thread, result);
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
          {        
            Platform::threadid_t thread_id = Platform::GetCurrentThreadId ();

            ThreadActionQueueMap::iterator iter = thread_queues.find (thread_id);

            if (iter == thread_queues.end ())
              return 0;

            ThreadActionQueuePtr queue = iter->second;

            return queue->Size ();
          }
          default:
            throw xtl::make_argument_exception ("", "thread", thread);
        }
      }
      catch (xtl::exception& e)
      {
        e.touch ("common::ActionQueue::QueueActionsCount");
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
          return default_action->GetWrapper ();
          
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
    
///Получение действия по умолчанию
    ActionPtr DefaultAction () { return default_action; }

  private:
///Получение очереди нити
    ThreadActionQueue& GetQueue (ActionThread thread)
    {
      switch (thread)
      {
        case ActionThread_Main:
          return main_thread_queue;
        case ActionThread_Background:
          return background_queue;
        case ActionThread_Current:
        {
          Platform::threadid_t thread_id = Platform::GetCurrentThreadId ();
          
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
      size_t end_time = common::milliseconds () + milliseconds;
      
      while (!action.IsCompleted () && !action.IsCanceled () && common::milliseconds () < end_time);

      return action.IsCompleted ();
    }

  private:
    typedef xtl::intrusive_ptr<ThreadActionQueue>                     ThreadActionQueuePtr;
    typedef stl::hash_map<Platform::threadid_t, ThreadActionQueuePtr> ThreadActionQueueMap;
    typedef xtl::signal<void (ActionThread, Action&)>                 Signal;

  private:
    Timer                       default_timer;
    Action::WaitCompleteHandler default_wait_handler;
    ActionPtr                   default_action;
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

Action ActionQueue::PushAction (const ActionHandler& action, ActionThread thread, time_t delay)
{
  ActionQueueSingleton::Instance queue;

  return queue->PushAction (action, thread, false, delay, 0.0, queue->DefaultTimer ());
}

Action ActionQueue::PushAction (const ActionHandler& action, ActionThread thread, time_t delay, time_t period)
{
  ActionQueueSingleton::Instance queue;

  return queue->PushAction (action, thread, true, delay, period, queue->DefaultTimer ());
}

Action ActionQueue::PushAction (const ActionHandler& action, ActionThread thread, time_t delay, Timer& timer)
{
  ActionQueueSingleton::Instance queue;

  return queue->PushAction (action, thread, false, delay, 0.0, timer);
}

Action ActionQueue::PushAction (const ActionHandler& action, ActionThread thread, time_t delay, time_t period, Timer& timer)
{
  ActionQueueSingleton::Instance queue;

  return queue->PushAction (action, thread, true, delay, period, timer);
}

Action ActionQueue::PushAction (const ActionHandler& action, const CallbackHandler& complete_callback, ActionThread thread, time_t delay)
{
  return PushAction (ActionWithCallback (action, complete_callback), thread, delay);
}

Action ActionQueue::PushAction (const ActionHandler& action, const CallbackHandler& complete_callback, ActionThread thread, time_t delay, Timer& timer)
{
  return PushAction (ActionWithCallback (action, complete_callback), thread, delay, timer);
}

size_t ActionQueue::ActionsCount (ActionThread thread)
{
  return ActionQueueSingleton::Instance ()->ActionsCount (thread);
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
  : impl (ActionQueueSingleton::Instance ()->DefaultAction ().get ())
{
}

Action::Action (ActionImpl* in_impl)
  : impl (in_impl)
{
  addref (impl);
}

Action::Action (const Action& action)
  : impl (action.impl)
{
  ActionLock lock (impl);

  addref (impl);
}

Action::~Action ()
{
  ActionLock lock (impl);

  release (impl);
}

Action& Action::operator = (const Action& action)
{
  Action (action).Swap (*this);

  return *this;
}

bool Action::IsEmpty () const
{
  return impl == ActionQueueSingleton::Instance ()->DefaultAction ().get ();
}

bool Action::IsCompleted () const
{
  return ActionLock (impl)->is_completed;
}

bool Action::IsCanceled () const
{
  return ActionLock (impl)->is_canceled;
}

bool Action::Wait (size_t milliseconds)
{
  try
  {
    return ActionLock (impl)->wait_handler (*this, milliseconds);
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
    ActionLock lock (impl);

    if (impl->is_completed)
      throw xtl::format_operation_exception ("", "Action already completed");

    if (impl->is_canceled)
      throw xtl::format_operation_exception ("", "Action already canceled");

    impl->action_handler (*this);

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
  ActionLock (impl)->is_canceled = true;
}

void Action::SetWaitHandler (const WaitCompleteHandler& handler)
{
  ActionLock (impl)->wait_handler = handler;
}

const Action::WaitCompleteHandler& Action::WaitHandler () const
{
  return ActionLock (impl)->wait_handler;
}

void Action::Swap (Action& action)
{
  stl::swap (impl, action.impl);
}

namespace common
{

void swap (Action& action1, Action& action2)
{
  action1.Swap (action2);
}

}
