#include "shared.h"

using namespace syslib;
using namespace xtl;
using namespace common;

typedef xtl::com_ptr<IApplicationDelegate> DelegatePtr;

namespace
{

const char* LOG_NAME = "system.application"; //имя потока протоколирования

}

/*
    Описание реализации приложения
*/

class ApplicationImpl: private IApplicationListener
{
  public:  
///Конструктор
    ApplicationImpl ()
      : log (LOG_NAME)
    {
      exit_code          = 0;
      message_loop_count = false;
      is_exit_detected   = false;
      on_push_action     = ActionQueue::RegisterEventHandler (ActionQueueEvent_OnPushAction, xtl::bind (&ApplicationImpl::OnPushAction, this, _1, _2));
      main_thread_id     = syslib::Platform::GetCurrentThreadId ();

      UpdateIdleState ();
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
          case ApplicationEvent_OnInitialize:
          case ApplicationEvent_OnPause:
          case ApplicationEvent_OnResume:
          case ApplicationEvent_OnStop:
          case ApplicationEvent_OnStart:
          case ApplicationEvent_OnMemoryWarning:
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

///Подписка на сообщения / посылка сообщений
    connection RegisterNotificationHandler (const char* notification_wildcard, const Application::NotificationHandler& handler)
    {
      return notification_signal.connect (xtl::bind (&ApplicationImpl::HandleNotification, _1, stl::string (notification_wildcard), handler));
    }

    void PostNotification (const char* notification)
    {
      notification_signal (notification);
    }
    
///Подписка на событие обновления системных свойств
    PropertyMap& CustomProperties () { return custom_properties; }

///Включен ли Idle режим?
    bool IsIdleEnabled ()
    {
      return !signals [ApplicationEvent_OnIdle].empty () || ActionQueue::ActionsCount (ActionThread_Main) != 0 || ActionQueue::ActionsCount (main_thread_id) != 0;
    }

///Обработка действий
    static void ProcessActions (ActionThread thread)
    {
      try
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
          catch (std::exception& e)
          {
            common::Log (LOG_NAME).Printf ("Exception in Application::ProcessActions: %s", e.what ());
          }
          catch (...)
          {
            common::Log (LOG_NAME).Printf ("Unknown exception in Application::ProcessActions");
          }
        }
      }
      catch (...)
      {
      }
    }

  private:
    typedef xtl::signal<void ()>             ApplicationSignal;
    typedef xtl::signal<void (const char*)>  NotificationSignal;
    
///Оповещение о возникновении события
    void Notify (ApplicationEvent event)
    {
      try
      {
        signals [event] ();
      }
      catch (xtl::exception& e)
      {
        log.Printf ("Exception in Application::Notify: %s", e.what ());
      }
      catch (...)
      {
        log.Printf ("Unknown exception in Application::Notify");
      }
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
    void OnInitialize ()
    {
      try
      {
        Notify (ApplicationEvent_OnInitialize);
      }
      catch (...)
      {
        //подавление всех исключений
      }
    }
    
///Обработка приостановки приложения
    void OnPause ()
    {
      try
      {
        Notify (ApplicationEvent_OnPause);
      }
      catch (...)
      {
        //подавление всех исключений
      }
    }

///Обработка восстановления приложения
    void OnResume ()
    {
      try
      {
        Notify (ApplicationEvent_OnResume);
      }
      catch (...)
      {
        //подавление всех исключений
      }
    }

///Обработка остановки приложения
    void OnStop ()
    {
      try
      {
        Notify (ApplicationEvent_OnStop);
      }
      catch (...)
      {
        //подавление всех исключений
      }
    }

///Обработка восстановления приложения
    void OnStart ()
    {
      try
      {
        Notify (ApplicationEvent_OnStart);
      }
      catch (...)
      {
        //подавление всех исключений
      }
    }

///Обработка недостаточности памяти
    void OnMemoryWarning ()
    {
      try
      {
        Notify (ApplicationEvent_OnMemoryWarning);
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
      //TODO: synchronization for UpdateIdleState

      switch (thread)
      {
        case ActionThread_Current:
          if (action.CreatorThreadId () != main_thread_id)
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

///Обработка сообщений
    static void HandleNotification (const char* notification, const stl::string& notification_wildcard, const Application::NotificationHandler& handler)
    {
      if (!common::wcmatch (notification, notification_wildcard.c_str ()))
        return;

      ActionQueue::PushAction (xtl::bind (&ApplicationImpl::ReportNotification, stl::string (notification), handler), ActionThread_Main);
    }
    
    static void ReportNotification (const stl::string& notification, const Application::NotificationHandler& handler)
    {
      handler (notification.c_str ());
    }

  private:
    common::Log             log;                            //протоколирование
    DelegatePtr             current_delegate;               //текущий делегат приложения
    ApplicationSignal       signals [ApplicationEvent_Num]; //сигналы приложения
    NotificationSignal      notification_signal;            //сигнал сообщений
    PropertyMap             custom_properties;              //пользовательские свойства
    int                     exit_code;                      //код завершения приложения
    bool                    is_exit_detected;               //получен сигнал завершения приложения
    size_t                  message_loop_count;             //количество вхождений в обработчик очереди сообщений
    xtl::auto_connection    on_push_action;                 //в очередь добавлено действие
    size_t                  main_thread_id;                 //идентификатор главной нити
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

        delegate->SetIdleState (app->IsIdleEnabled ());
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

void Application::ProcessThreadMessages ()
{
  try
  {
    Platform::ProcessThreadMessages ();

    ApplicationImpl::ProcessActions (ActionThread_Current);
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::Application::ProcessThreadMessages");
    throw;
  }  
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

connection Application::RegisterNotificationHandler (const char* notification_wildcard, const NotificationHandler& handler)
{
  if (!notification_wildcard)
    throw xtl::make_null_argument_exception ("syslib::Application::RegisterNotificationHandler", "notification_wildcard");

  return ApplicationSingleton::Instance ()->RegisterNotificationHandler (notification_wildcard, handler);
}

void Application::PostNotification (const char* notification)
{
  if (!notification)
    throw xtl::make_null_argument_exception ("syslib::Application::PostNotification", "notification");

  ApplicationSingleton::Instance ()->PostNotification (notification);
}

void Application::Sleep (size_t milliseconds)
{
  Platform::Sleep (milliseconds);
}

void Application::OpenUrl (const char* url)
{
  try
  {
    if (!url)
      throw xtl::make_null_argument_exception ("", "url");

    Platform::OpenUrl (url);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::Application::OpenUrl");
    throw;
  }
}

stl::string Application::GetEnvironmentVariable (const char* name)
{
  try
  {
    if (!name)
      throw xtl::make_null_argument_exception ("", "name");
      
    return Platform::GetEnvironmentVariable (name);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::Application::GetEnvironmentVariable");
    throw;
  }  
}

void Application::SetScreenSaverState (bool state)
{
  try
  {
    return Platform::SetScreenSaverState (state);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::Application::SetScreenSaverState");
    throw;
  }  
}

bool Application::ScreenSaverState ()
{
  try
  {
    return Platform::GetScreenSaverState ();
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::Application::ScreenSaverState");
    throw;
  }  
}

/*
    Управление режимом работы в фоне
*/

void Application::SetBackgroundMode (ApplicationBackgroundMode mode)
{
  try
  {
    switch (mode)
    {
      case ApplicationBackgroundMode_Active:
      case ApplicationBackgroundMode_Suspend:
        break;
      default:
        throw xtl::make_argument_exception ("", "mode", mode);
    }

    Platform::SetApplicationBackgroundMode (mode);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::Application::SetBackgroundMode");
    throw;
  }
}

ApplicationBackgroundMode Application::BackgroundMode ()
{
  try
  {
    return Platform::GetApplicationBackgroundMode ();
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::Application::BackgroundMode");
    throw;
  }
}

/*
    Получение платформо-зависимых системных свойств
*/

common::PropertyMap Application::SystemProperties ()
{
  try
  {
    common::PropertyMap properties;
    
    Platform::GetSystemProperties (properties);

    ApplicationSingleton::Instance instance;

    PropertyMap& custom_properties = instance->CustomProperties ();

    for (size_t i=0, count=custom_properties.Size (); i<count; i++)
      properties.SetProperty (custom_properties.PropertyName (i), custom_properties.GetString (i));

    return properties;
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::Application::SystemProperties");
    throw;
  }
}

void Application::SetSystemProperty (const char* name, const char* value)
{
  try
  {
    ApplicationSingleton::Instance instance;
    
    PropertyMap& custom_properties = instance->CustomProperties ();
    
    custom_properties.SetProperty (name, value);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::Application::SetSystemProperty");
    throw;
  }
}

void Application::RemoveSystemProperty (const char* name)
{
  try
  {
    ApplicationSingleton::Instance instance;
    
    PropertyMap& custom_properties = instance->CustomProperties ();
    
    custom_properties.RemoveProperty (name);
  }
  catch (...)
  {
    //подавление всех исключений
  }
}
