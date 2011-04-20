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
    void BeginDelegate (IApplicationDelegate* delegate)
    {          
      try
      {
        if (!delegate)
          throw xtl::make_null_argument_exception ("", "delegate");
          
        if (current_delegate)
          throw xtl::format_operation_exception ("", "Application has delegate already");
          
        current_delegate = delegate;

        try
        {
          delegate->SetListener (this);
            
          if (is_exit_detected)
            delegate->Exit (exit_code);

          UpdateIdleState ();
        }
        catch (...)
        {
          current_delegate = 0;
          throw;
        }
      }
      catch (xtl::exception& e)
      {
        e.touch ("syslib::Application::BeginDelegate");
        throw;
      }
    }

    void EndDelegate ()
    {
      try
      {
        if (!current_delegate)
          return;
          
        current_delegate = 0;
      }
      catch (xtl::exception& e)
      {
        e.touch ("syslib::Application::EndDelegate");
        throw;
      }
    }

///Работа с очередью сообщений
    void EnterMessageLoop ()
    {            
      if (!message_loop_count++)
      {
        Notify (ApplicationEvent_OnRunLoopEnter);
      }
    }
    
    void ExitMessageLoop ()
    {
      if (!--message_loop_count)
      {
        Notify (ApplicationEvent_OnRunLoopExit);
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

      Notify (ApplicationEvent_OnExit);
      
      try
      {
        if (current_delegate)
          current_delegate->Exit (code);
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
          case ApplicationEvent_OnExit:
          case ApplicationEvent_OnRunLoopEnter:
          case ApplicationEvent_OnRunLoopExit:
          case ApplicationEvent_OnInitialized:
            return signals [event].connect (handler);
          case ApplicationEvent_OnIdle:
          {
            connection return_value = signals [ApplicationEvent_OnIdle].connect (handler);
            UpdateIdleState ();
            return return_value;
          }
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
    void Notify (ApplicationEvent event)
    {
      try
      {
        signals [event] ();
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
      return !signals [ApplicationEvent_OnIdle].empty () || ActionQueue::ActionsCount (ActionThread_Main) != 0 || ActionQueue::ActionsCount (ActionThread_Current) != 0;
    }

///Обновление idle-режима
    void UpdateIdleState ()
    {
      try
      {
        if (current_delegate)
          current_delegate->SetIdleState (IsIdleEnabled ());
      }
      catch (xtl::exception& e)
      {
        e.touch ("syslib::ApplicationImpl::UpdateIdleState");
        throw;
      }
    }

///Обработка инициализации приложения
    void OnInitialized ()
    {
      try
      {
        Notify (ApplicationEvent_OnInitialized);
      }
      catch (...)
      {
        //подавление всех исключений
      }
    }
    
///Обработка idle
    void OnIdle ()
    {
      try
      {
        if (is_exit_detected)
          return;
        
        ProcessActions (ActionThread_Main);
        ProcessActions (ActionThread_Current);

        Notify (ApplicationEvent_OnIdle);
      
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
    DelegatePtr          current_delegate;               //текущий делегат приложения        
    ApplicationSignal    signals [ApplicationEvent_Num]; //сигналы приложения
    int                  exit_code;                      //код завершения приложения
    bool                 is_exit_detected;               //получен сигнал завершения приложения
    size_t               message_loop_count;             //количество вхождений в обработчик очереди сообщений
    xtl::auto_connection on_push_action;                 //в очередь добавлено действие
};

typedef Singleton<ApplicationImpl> ApplicationSingleton;

/*
    Обёртки над вызовами
*/

void Application::BeginDelegate (IApplicationDelegate* delegate)
{
  ApplicationSingleton::Instance ()->BeginDelegate (delegate);
}

void Application::EndDelegate ()
{
  ApplicationSingleton::Instance ()->EndDelegate ();
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
      
        app->BeginDelegate (delegate.get ());
      
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
      ApplicationSingleton::Instance ()->EndDelegate ();
      throw;
    }      

    {
      ApplicationSingleton::Instance app;
      
      app->ExitMessageLoop ();      
      app->EndDelegate ();
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
