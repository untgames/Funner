#include "shared.h"

using namespace syslib;
using namespace xtl;
using namespace common;

typedef xtl::com_ptr<IApplicationDelegate> DelegatePtr;

/*
    Описание реализации приложения
*/

class ApplicationImpl: private IApplicationListener
{
  public:  
///Конструктор
    ApplicationImpl ()
    {
      exit_code          = 0;
      message_loop_count = false;
      is_exit_detected   = false;
      on_push_action     = ActionQueue::RegisterEventHandler (ActionQueueEvent_OnPushAction, xtl::bind (&ApplicationImpl::OnPushAction, this, _1, _2));
    }      
    
///Работа с делегатами
    void PushDelegate (IApplicationDelegate* delegate)
    {          
      try
      {
        if (!delegate)
          throw xtl::make_null_argument_exception ("", "delegate");                  

        delegates.push (delegate);
        
        try
        {
          delegate->SetListener (this);
          
          if (is_exit_detected)
            delegate->Exit (exit_code);

          UpdateIdleState ();
        }
        catch (...)
        {
          delegates.pop ();
          throw;
        }
      }
      catch (xtl::exception& e)
      {
        e.touch ("syslib::Application::PushDelegate");
        throw;
      }
    }

    void PopDelegate ()
    {
      try
      {
        if (delegates.empty ())
          throw xtl::format_operation_exception ("", "No delegates in stack");

        delegates.pop ();      

        UpdateIdleState ();
      }
      catch (xtl::exception& e)
      {
        e.touch ("syslib::Application::PopDelegate");
        throw;
      }
    }

///Работа с очередью сообщений
    void EnterMessageLoop ()
    {            
      if (!message_loop_count++)
      {
        Notify (enter_run_loop_signal);        
      }
    }
    
    void ExitMessageLoop ()
    {
      if (!--message_loop_count)
      {
        Notify (exit_run_loop_signal);        
      }
    }
    
    bool IsMessageLoop () const
    {
      return message_loop_count > 0;
    }    
    
///Прекращение работы приложения
    void Exit (int code, bool need_notify_delegate = true)
    {
      if (is_exit_detected)
        return;

      is_exit_detected = true;
      exit_code        = code;

      Notify (exit_signal);
      
      try
      {
        if (!delegates.empty ())
          delegates.top ()->Exit (code);
      }
      catch (...)
      {
        ///подавление всех исключений
      }
    }
        
    int GetExitCode () const
    {
      return exit_code; 
    }

///Подписка на события приложения
    connection RegisterEventHandler (ApplicationEvent event, const Application::EventHandler& handler)
    {
      try
      {
        switch (event)
        {
          case ApplicationEvent_OnExit:         return exit_signal.connect (handler);
          case ApplicationEvent_OnRunLoopEnter: return enter_run_loop_signal.connect (handler);
          case ApplicationEvent_OnRunLoopExit:  return exit_run_loop_signal.connect (handler);
          case ApplicationEvent_OnIdle:
            UpdateIdleState ();
            return idle_signal.connect (handler);        
          default:
            throw xtl::make_argument_exception ("", "event", event);
        }
      }
      catch (xtl::exception& e)
      {
        e.touch ("syslib::Application::RegisterEventHandler");
        throw;
      }
    }

  private:
    typedef xtl::signal<void ()> ApplicationSignal;        
    
///Оповещение о возникновении события
    void Notify (ApplicationSignal& signal)
    {
      try
      {
        signal ();
      }
      catch (...)
      {
        //подавление всех исключений
      }
    }          
    
///Обработка действий
    void ProcessActions (ActionThread thread)
    {
      size_t count = ActionQueue::ActionsCount (thread);
      
      for (size_t i=0; i<count; i++)
      {
        try
        {
          Action action = ActionQueue::PopAction (thread);

          if (action.IsEmpty ())
            continue;
            
          action.Perform ();
        }
        catch (...)
        {
          //подавление исключений
        }
      }  
    }    
    
///Включен ли Idle режим?
    bool IsIdleEnabled ()
    {
      return !idle_signal.empty () || ActionQueue::ActionsCount (ActionThread_Main) != 0 || ActionQueue::ActionsCount (ActionThread_Current) != 0;
    }

///Обновление idle-режима
    void UpdateIdleState ()
    {
      try
      {
        if (!delegates.empty ())
          delegates.top ()->SetIdleState (IsIdleEnabled ());
      }
      catch (xtl::exception& e)
      {
        e.touch ("syslib::ApplicationImpl::UpdateIdleState");
        throw;
      }
    }    
    
///Обработка idle
    void OnIdle ()
    {
      try
      {
        ProcessActions (ActionThread_Main);
        ProcessActions (ActionThread_Current);

        Notify (idle_signal);
      
        UpdateIdleState ();
      }
      catch (...)
      {
        ///подавление всех исключений
      }
    }
    
///Обработка событий exit
    void OnExit (int code)
    {
      try
      {
        Exit (code, false);
      }
      catch (...)
      {
        ///подавление всех исключений
      }
    }
       
///Событие добавлено в очередь событий
    void OnPushAction (ActionThread thread, Action& action)
    {
      switch (thread)
      {
        case ActionThread_Current:
          if (action.CreaterThreadId () != Platform::GetCurrentThreadId ())
            return;        
        case ActionThread_Main:
        {
          try
          {
            UpdateIdleState ();
          }
          catch (...)
          {
          }

          break;
        }
        default:
          break;
      }              
    }    
    
  private:
    typedef stl::stack<DelegatePtr> DelegateStack;

  private:
    DelegateStack        delegates;             //делегаты приложения
    ApplicationSignal    idle_signal;           //сигнал обработки события ApplicationEvent_OnIdle
    ApplicationSignal    exit_signal;           //сигнал обработки события ApplicationEvent_OnExit
    ApplicationSignal    enter_run_loop_signal; //сигнал обработки события ApplicationEvent_OnRunLoopEnter
    ApplicationSignal    exit_run_loop_signal;  //сигнал обработки события ApplicationEvent_OnRunLoopExit
    int                  exit_code;             //код завершения приложения
    bool                 is_exit_detected;      //получен сигнал завершения приложения
    size_t               message_loop_count;    //количество вхождений в обработчик очереди сообщений
    xtl::auto_connection on_push_action;        //в очередь добавлено действие
};

typedef Singleton<ApplicationImpl> ApplicationSingleton;

/*
    Обёртки над вызовами
*/

void Application::PushDelegate (IApplicationDelegate* delegate)
{
  ApplicationSingleton::Instance ()->PushDelegate (delegate);
}

void Application::PopDelegate ()
{
  ApplicationSingleton::Instance ()->PopDelegate ();
}

void Application::Run (IApplicationDelegate* in_delegate)
{
  try
  {
    DelegatePtr delegate = in_delegate;
    
    if (!delegate)
      delegate = DelegatePtr (Platform::CreateDefaultApplicationDelegate (), false);
      
    try
    {
      {
        ApplicationSingleton::Instance app;
      
        app->PushDelegate (delegate.get ());
      
        app->EnterMessageLoop ();
      }

      try
      {
        delegate->Run ();
      }
      catch (...)
      {
        ApplicationSingleton::Instance ()->ExitMessageLoop ();
        throw;
      }
    }
    catch (...)
    {
      ApplicationSingleton::Instance ()->PopDelegate ();
      throw;
    }      

    {
      ApplicationSingleton::Instance app;
      
      app->ExitMessageLoop ();      
      app->PopDelegate ();
    }
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::Application::Run");
    throw;
  }
}

bool Application::IsMessageLoop ()
{
  return ApplicationSingleton::Instance ()->IsMessageLoop ();
}

void Application::Exit (int code)
{
  ApplicationSingleton::Instance ()->Exit (code);
}

int Application::GetExitCode ()
{
  return ApplicationSingleton::Instance ()->GetExitCode ();
}

connection Application::RegisterEventHandler (ApplicationEvent event, const EventHandler& handler)
{
  return ApplicationSingleton::Instance ()->RegisterEventHandler (event, handler);
}

void Application::Sleep (size_t milliseconds)
{
  Platform::Sleep (milliseconds);
}
