#import <AppKit/NSApplication.h>
#import <Foundation/NSAutoreleasePool.h>

#include "shared.h"

using namespace syslib;

namespace
{

const double RECEIVE_EVENT_TIMEOUT = 0.000000001;

class CarbonApplicationDelegate: public IApplicationDelegate, public xtl::reference_counter
{
  public:
///Конструктор
    CarbonApplicationDelegate ()
      : idle_enabled (false), is_exited (false), listener (0), main_thread_id (0), dummy_event (0)
    {
      check_event_manager_error (CreateEvent (0, 0, 0, 0, kEventAttributeNone, &dummy_event), "::CreateEvent", "Can't create dummy event");
    }

    ~CarbonApplicationDelegate ()
    {
      if (dummy_event)
        ReleaseEvent (dummy_event);
    }

///Запуск цикла обработки сообщений
    void Run ()
    {
      NSApplicationLoad ();

      NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];

      try
      {
        main_thread_id = Platform::GetCurrentThreadId ();
        
        if (listener)
          listener->OnInitialized ();

        while (!is_exited)
        {
          EventRef next_event;

          ReceiveNextEvent (0, 0, RECEIVE_EVENT_TIMEOUT, false, &next_event);

          while (!IsMessageQueueEmpty ())
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
        [pool release];
        e.touch ("syslib::CarbonApplicationDelegate::Run");
        throw;
      }

      [pool release];
    }

///Выход из приложения
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
      idle_enabled = state;

      if (state && IsMessageQueueEmpty ())
        check_event_manager_error (PostEventToQueue (GetCurrentEventQueue (), dummy_event, kEventPriorityStandard),
                                   "::PostEventToQueue", "Can't post dummy event");
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

      return GetNumEventsInQueue (GetCurrentEventQueue ()) == 0;
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

      if (GetEventClass (event) == kEventClassApplication)
        if (GetEventKind (event) == kEventAppQuit)
        {
          ReleaseEvent (event);
          is_exited = true;
          return;
        }

      try
      {
        OSStatus operation_result = SendEventToEventTarget (event, GetEventDispatcherTarget ());

        if (operation_result != eventNotHandledErr)
          check_event_manager_error (operation_result, "::SendEventToEventTarget", "Can't dispatch event");
      }
      catch (xtl::exception& exception)
      {
        ReleaseEvent (event);

        throw xtl::format_operation_exception ("syslib::CarbonApplicationDelegate::DoNextEvent", "%s", exception.what ());
      }

      ReleaseEvent (event);
    }

  private:
    bool                  idle_enabled;
    bool                  is_exited;
    IApplicationListener* listener;
    size_t                main_thread_id;
    EventRef              dummy_event;     //событие, посылаемое для пробуждения нити
};

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
