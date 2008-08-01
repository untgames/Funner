#include "shared.h"

using namespace syslib;
using namespace xtl;
using namespace common;

namespace
{

/*
    Враппер для упрощения подсчёта вхождений в обработчик очереди сообщений
*/

class MessageLoopScope
{
  public:
    MessageLoopScope  (size_t& in_counter) : counter (in_counter) { counter++; }
    ~MessageLoopScope () { counter--; }
    
  private:
    size_t& counter;
};

/*
    Аккумулятор сигнала с операцией OR
*/

struct or_signal_accumulator
{
  typedef bool result_type;
  
  template <class InIter> bool operator () (InIter first, InIter last) const
  {
    for (; first!=last; ++first)
      if (*first)
        return true;

    return false;
  }
};

}

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
    void DoEvents ();

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
    connection RegisterEventHandler   (ApplicationEvent event, const Application::EventHandler& handler);
    connection RegisterSuspendHandler (const Application::SuspendHandler& handler);
    
  private:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Оповещение о возникновении события
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Notify (ApplicationEvent);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Засыпание
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Suspend ();
  
  private:
    typedef xtl::signal<void ()>                        ApplicationSignal;
    typedef xtl::signal<bool (), or_signal_accumulator> SuspendSignal;

    ApplicationSignal  signals [ApplicationEvent_Num]; //сигналы приложения
    SuspendSignal      suspend_handler;                //сигнал проверки наличия необработанных сообщений
    int                exit_code;                      //код завершения приложения
    bool               is_exit_detected;               //получен сигнал завершения приложения
    size_t             message_loop_count;             //количество вхождений в обработчик очереди сообщений
};

typedef Singleton<ApplicationImpl> ApplicationSingleton;

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
    Засыпание
*/

void ApplicationImpl::Suspend ()
{
  try
  {
    if (!suspend_handler ())
      return;

    if (!is_exit_detected)
      Platform::WaitMessage ();
  }
  catch (...)
  {
    //подавление всех исключений
  }
}

/*
    Обработка сообщений в очереди сообщений
*/

void ApplicationImpl::DoEvents ()
{
  MessageLoopScope scope (message_loop_count);

  while (!Platform::IsMessageQueueEmpty () && !is_exit_detected)
    Platform::DoNextEvent ();

  if (is_exit_detected)
    return;

  try
  {
    signals [ApplicationEvent_OnDoEvents] ();
  }
  catch (...)
  {
    //подавление всех исключений
  }
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
      if (!is_exit_detected && Platform::IsMessageQueueEmpty ())
        Suspend ();
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
    throw xtl::make_argument_exception ("syslib::Application::RegisterEventHandler", "event", event);

  return signals [event].connect (handler);
}

connection ApplicationImpl::RegisterSuspendHandler (const Application::SuspendHandler& handler)
{
  return suspend_handler.connect (handler);
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

connection Application::RegisterSuspendHandler (const SuspendHandler& handler)
{
  return ApplicationSingleton::Instance ().RegisterSuspendHandler (handler);
}

void Application::Sleep (size_t milliseconds)
{
  Platform::Sleep (milliseconds);
}
