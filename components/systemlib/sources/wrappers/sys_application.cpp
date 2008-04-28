#include <syslib/application.h>
#include <xtl/signal.h>
#include <common/singleton.h>
#include <common/exception.h>
#include <platform/platform.h>

using namespace syslib;
using namespace xtl;
using namespace common;

/*
    Описание реализации приложения
*/

class ApplicationImpl
{
  public:
    ApplicationImpl ();    
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Обработка сообщений в очереди сообщений
///////////////////////////////////////////////////////////////////////////////////////////////////
    void DoNextEvent ();
    void DoEvents    ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Запуск обработки очереди сообщений
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Run ();
    bool IsMessageLoop () const { return message_loop_count > 0; }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Прекращение обработки очереди сообщений
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Exit        (int code);
    int  GetExitCode () const { return exit_code; }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Подписка на события приложения
///////////////////////////////////////////////////////////////////////////////////////////////////
    connection RegisterEventHandler (ApplicationEvent event, const Application::EventHandler& handler);
    
  private:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Оповещение о возникновении события
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Notify (ApplicationEvent);
  
  private:
    typedef signal<void ()> ApplicationSignal;

    ApplicationSignal signals [ApplicationEvent_Num]; //сигналы приложения
    int               exit_code;                      //код завершения приложения
    bool              is_exit_detected;               //получен сигнал завершения приложения
    size_t            message_loop_count;             //количество вхождений в обработчик очереди сообщений
};

typedef Singleton<ApplicationImpl> ApplicationSingleton;

/*
    Враппер для упрощения подсчёта вхождений в обработчик очереди сообщений
*/

namespace
{

class MessageLoopScope
{
  public:
    MessageLoopScope  (size_t& in_counter) : counter (in_counter) { counter++; }
    ~MessageLoopScope () { counter--; }
    
  private:
    size_t& counter;
};

}

/*
    Конструктор
*/

ApplicationImpl::ApplicationImpl ()
{
  exit_code          = 0;
  message_loop_count = false;
  is_exit_detected   = false;
}

/*
    Прекращение обработки очереди сообщений
*/

void ApplicationImpl::Exit (int code)
{
  if (is_exit_detected)
    return;

  is_exit_detected = true;
  exit_code        = code;
  
  Notify (ApplicationEvent_OnExit);
}

/*
    Обработка сообщений в очереди сообщений
*/

void ApplicationImpl::DoNextEvent ()
{
  if (is_exit_detected)
    return;

  MessageLoopScope scope (message_loop_count);
  
  Platform::DoNextEvent ();
}

void ApplicationImpl::DoEvents ()
{
  MessageLoopScope scope (message_loop_count);

  while (!Platform::IsMessageQueueEmpty () && !is_exit_detected)
    Platform::DoNextEvent ();
}

void ApplicationImpl::Run ()
{
  MessageLoopScope scope (message_loop_count);

  while (!is_exit_detected)
  {
    DoEvents ();

     //если нет обработчиков OnIdle - приостанавливаем приложение

    if (signals [ApplicationEvent_OnIdle].empty ())
    {
      if (!is_exit_detected)
        Platform::WaitMessage ();
    }
    else
    {
      Notify (ApplicationEvent_OnIdle);
    }
  }
}

/*
    Подписка на события приложения
*/

connection ApplicationImpl::RegisterEventHandler (ApplicationEvent event, const Application::EventHandler& handler)
{
  if (event < 0 || event >= ApplicationEvent_Num)
    RaiseInvalidArgument ("syslib::Application::RegisterEventHandler", "event", event);

  return signals [event].connect (handler);
}

/*
    Оповещение о событиях приложения
*/

void ApplicationImpl::Notify (ApplicationEvent event)
{
  signals [event] ();
}

/*
    Обёртки над вызовами
*/

void Application::DoNextEvent ()
{
  ApplicationSingleton::Instance ().DoNextEvent ();
}

void Application::DoEvents ()
{
  ApplicationSingleton::Instance ().DoEvents ();
}

void Application::Run ()
{
  ApplicationSingleton::Instance ().Run ();
}

bool Application::IsMessageLoop ()
{
  return ApplicationSingleton::Instance ().IsMessageLoop ();
}

void Application::Exit (int code)
{
  ApplicationSingleton::Instance ().Exit (code);
}

int Application::GetExitCode ()
{
  return ApplicationSingleton::Instance ().GetExitCode ();
}

connection Application::RegisterEventHandler (ApplicationEvent event, const EventHandler& handler)
{
  return ApplicationSingleton::Instance ().RegisterEventHandler (event, handler);
}

void Application::Sleep (size_t milliseconds)
{
  Platform::Sleep (milliseconds);
}
