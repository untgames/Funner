#import <AppKit/NSApplication.h>
#import <Foundation/NSAutoreleasePool.h>

#include "shared.h"

using namespace syslib;
using namespace syslib::macosx;

namespace
{

const char* LOG_NAME = "syslib.CarbonApplication";

const double RECEIVE_EVENT_TIMEOUT         = 0.000000001;
const UInt32 UNTGS_EVENT_CLASS             = 'untg';      //класс событий приложения
const UInt32 APPLICATION_LOOP_START_EVENT  = 'alse';      //событие входа в цикл обработки
const size_t UPDATE_SYSTEM_ACTIVITY_PERIOD = 30 * 1000;   //период обновления активности системы для предотвращения запуска хранителя экрана

OSStatus application_event_handler_func (EventHandlerCallRef event_handler_call_ref, EventRef event, void* application_delegate);

class CarbonApplicationDelegate: public IApplicationDelegate, public xtl::reference_counter
{
  public:
///Конструктор
    CarbonApplicationDelegate ()
      : idle_enabled (false)
      , is_exited (false)
      , event_loop_started (false)
      , listener (0)
      , main_thread_id (0)
      , application_event_handler (0)
      , application_event_handler_proc (0)
      {}

    ~CarbonApplicationDelegate ()
    {
      if (application_event_handler)
        RemoveEventHandler (application_event_handler);

      if (application_event_handler_proc)
        DisposeEventHandlerUPP (application_event_handler_proc);
    }

///Запуск цикла обработки сообщений
    void Run ()
    {
      static const char* METHOD_NAME = "syslib::CarbonApplicationDelegate::Run";

      if (application_event_handler)
        throw xtl::format_operation_exception (METHOD_NAME, "Application already runned");

      EventRef first_event = 0;

      try
      {
        EventTypeSpec application_handled_event_types [] = {
          { UNTGS_EVENT_CLASS, APPLICATION_LOOP_START_EVENT },
          { kEventClassApplication, kEventAppQuit },
        };

        EventHandlerRef application_event_handler;

        EventHandlerUPP application_event_handler_proc = NewEventHandlerUPP (&application_event_handler_func);

        check_event_manager_error (InstallApplicationEventHandler (application_event_handler_proc,
                                   sizeof (application_handled_event_types) / sizeof (application_handled_event_types[0]),
                                   application_handled_event_types, this, &application_event_handler),
                                   "::InstallApplicationEventHandler", "Can't install event handler");

        check_event_manager_error (CreateEvent (0, UNTGS_EVENT_CLASS, APPLICATION_LOOP_START_EVENT, 0, kEventAttributeNone, &first_event),
                                   "::CreateEvent", "Can't create first event");
        check_event_manager_error (PostEventToQueue (GetMainEventQueue (), first_event, kEventPriorityStandard),
                                   "::PostEventToQueue", "Can't post first event");

        RunApplicationEventLoop ();
      }
      catch (xtl::exception& e)
      {
        if (first_event)
          ReleaseEvent (first_event);

        e.touch (METHOD_NAME);
        throw;
      }

      if (first_event)
        ReleaseEvent (first_event);
    }

    //Старт главного цикла
    void OnApplicationEventLoopStarted ()
    {
      NSApplicationLoad ();

      NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];

      try
      {
        main_thread_id = Platform::GetCurrentThreadId ();

        event_loop_started = true;

        if (listener)
          listener->OnInitialized ();

        while (!is_exited)
        {
          EventRef next_event;

          ReceiveNextEvent (0, 0, RECEIVE_EVENT_TIMEOUT, false, &next_event);

          while (!is_exited && !IsMessageQueueEmpty ())
            DoNextEvent ();

           //если нет обработчиков OnIdle - приостанавливаем приложение

          if (!idle_enabled)
          {
            if (IsMessageQueueEmpty () && !is_exited)
            {
              WaitMessage ();
            }
          }
          else
          {
            if (listener)
              listener->OnIdle ();
          }
        }
      }
      catch (xtl::exception& e)
      {
        common::Log (LOG_NAME).Printf ("%s\n    at syslib::CarbonApplicationDelegate::OnApplicationEventLoopStarted", e.what ());
        QuitApplicationEventLoop ();
      }
      catch (...)
      {
        common::Log (LOG_NAME).Printf ("Unknown exception\n    at syslib::CarbonApplicationDelegate::OnApplicationEventLoopStarted");
        QuitApplicationEventLoop ();
      }

      [pool release];
    }

    //Событие выхода из приложения
    void OnExit ()
    {
      is_exited = true;
    }

    //Выход из приложения
    void Exit (int code)
    {
      EventRef application_exit_event = 0;

      try
      {
        check_event_manager_error (CreateEvent (0, kEventClassApplication, kEventAppQuit, 0, kEventAttributeUserEvent, &application_exit_event),
                                   "::CreateEvent", "Can't create application exit event");

        check_event_manager_error (PostEventToQueue (GetMainEventQueue (), application_exit_event, kEventPriorityStandard),
                                   "::PostEventToQueue", "Can't post application exit event");
      }
      catch (xtl::exception& e)
      {
        if (application_exit_event)
          ReleaseEvent (application_exit_event);

        e.touch ("syslib::CarbonApplicationDelegate::Exit");
        throw;
      }

      if (application_exit_event)
        ReleaseEvent (application_exit_event);
    }

///Установка необходимости вызова событий idle
    void SetIdleState (bool state)
    {
      if (idle_enabled == state)
        return;

      idle_enabled = state;

      if (state && event_loop_started && IsMessageQueueEmpty ()) //Нельзя добавлять сообщения в очередь до старта главного цикла (иногда события блокируются)
      {
        EventRef dummy_event;

        check_event_manager_error (CreateEvent (0, 0, 0, 0, kEventAttributeNone, &dummy_event), "::CreateEvent", "Can't create dummy event");

        check_event_manager_error (PostEventToQueue (GetMainEventQueue (), dummy_event, kEventPriorityStandard),
                                   "::PostEventToQueue", "Can't post dummy event");

        ReleaseEvent (dummy_event);
      }
    }

///Установка слушателя событий приложения
    void SetListener (IApplicationListener* in_listener)
    {
      listener = in_listener;
    }

///Подсчёт ссылок
    void AddRef ()
    {
      addref (this);
    }

    void Release ()
    {
      release (this);
    }

  private:
///Проверка очереди событий на пустоту
    bool IsMessageQueueEmpty ()
    {
      if (is_exited)
        return true;

      return GetNumEventsInQueue (GetMainEventQueue ()) == 0;
    }

///Ожидание события
    void WaitMessage ()
    {
      EventRef next_event;

      check_event_manager_error (ReceiveNextEvent (0, 0, kEventDurationForever, false, &next_event),
                                 "syslib::CarbonApplicationDelegate::WaitMessage", "Can't wait for next message, error at '::ReceiveNextEvent'");
    }

///Обработка следующего события
    void DoNextEvent ()
    {
      EventRef event;

      ReceiveNextEvent (0, 0, RECEIVE_EVENT_TIMEOUT, true, &event);

      if (!event)
        return;

      SendEventToEventTargetWithOptions (event, GetEventDispatcherTarget (), kEventTargetSendToAllHandlers); //ignore operation result, because it returns -1708 with RunApplicationEventLoop

      ReleaseEvent (event);
    }

  private:
    bool                  idle_enabled;
    bool                  is_exited;
    bool                  event_loop_started;
    IApplicationListener* listener;
    size_t                main_thread_id;
    EventHandlerRef       application_event_handler;
    EventHandlerUPP       application_event_handler_proc;
};

//Обработка первого события приложения с целью запуска главного цикла
OSStatus application_event_handler_func (EventHandlerCallRef event_handler_call_ref, EventRef event, void* application_delegate)
{
  CarbonApplicationDelegate* delegate = (CarbonApplicationDelegate*)application_delegate;

  if (GetEventClass (event) == kEventClassApplication && GetEventKind (event) == kEventAppQuit)
  {
    delegate->OnExit ();
    return eventNotHandledErr;
  }

  if (GetEventClass (event) != UNTGS_EVENT_CLASS || GetEventKind (event) != APPLICATION_LOOP_START_EVENT)
    return eventNotHandledErr;

  delegate->OnApplicationEventLoopStarted ();

  return noErr;
}

///Данные приложения
class ApplicationImpl
{
  public:
///Конструктор
    ApplicationImpl ()
      : screen_saver_state (true)
      , activity_update_timer (xtl::bind (&ApplicationImpl::UpdateActivity, this), UPDATE_SYSTEM_ACTIVITY_PERIOD, TimerState_Paused)
      {}

    ///Деструктор
    ~ApplicationImpl ()
    {
      try
      {
        if (!screen_saver_state)
          SetScreenSaverState (true);
      }
      catch (...)
      {
      }
    }

///Установка состояния скрин-сейвера
    void SetScreenSaverState (bool state)
    {
      if (state == screen_saver_state)
        return;

      if (state)
      {
        activity_update_timer.Pause ();
      }
      else
      {
        activity_update_timer.Run ();
        UpdateActivity ();
      }

      screen_saver_state = state;
    }

    bool GetScreenSaverState () { return screen_saver_state; }

  private:
    void UpdateActivity ()
    {
      UpdateSystemActivity (OverallAct);
    }

  private:
    bool  screen_saver_state;     //состояние скрин-сейвера
    Timer activity_update_timer;  //таймер обновления активности приложения для предотвращения запуска скрин-сейвера
};

typedef common::Singleton<ApplicationImpl> ApplicationSingleton;

}

/*
    Создание делегата приложения
*/

IApplicationDelegate* CarbonApplicationManager::CreateDefaultApplicationDelegate ()
{
  return new CarbonApplicationDelegate;
}

/*
   Открытие URL во внешнем браузере
*/

void CarbonApplicationManager::OpenUrl (const char* url)
{
  NSAutoreleasePool *pool          = [[NSAutoreleasePool alloc] init];
  NSString          *ns_url_string = [NSString stringWithUTF8String:url];
  NSURL             *ns_url        = [NSURL URLWithString:[ns_url_string stringByAddingPercentEscapesUsingEncoding:NSUTF8StringEncoding]];

  BOOL result = [[NSWorkspace sharedWorkspace] openURL:ns_url];

  [pool release];

  if (!result)
    throw xtl::format_operation_exception ("::NSWorkspace::openURL", "Can't open URL '%s'", url);
}

/*
   Управление энергосбережением
*/

void CarbonApplicationManager::SetScreenSaverState (bool state)
{
  try
  {
    ApplicationSingleton::Instance ()->SetScreenSaverState (state);
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::CarbonApplicationManager::SetScreenSaverState");
    throw;
  }
}

bool CarbonApplicationManager::GetScreenSaverState ()
{
  return ApplicationSingleton::Instance ()->GetScreenSaverState ();
}
