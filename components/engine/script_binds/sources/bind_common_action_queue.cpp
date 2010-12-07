#include "shared.h"

#include <xtl/connection.h>
#include <xtl/reference_counter.h>
#include <xtl/signal.h>

#include <common/time.h>

#include <syslib/application.h>

using namespace script;

namespace
{

const char* LOG_NAME                    = "script.binds.ActionQueue";
const char* COMMON_ACTION_QUEUE_LIBRARY = "Common.ActionQueue";

///////////////////////////////////////////////////////////////////////////////////////////////////
///Очередь действий
///////////////////////////////////////////////////////////////////////////////////////////////////
class ActionQueue: public xtl::reference_counter, public xtl::dynamic_cast_root
{
  public:
    typedef xtl::intrusive_ptr<ActionQueue> Pointer;
    typedef xtl::function<void (float)>     EventHandler;  //dt

///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    ActionQueue ()
      : paused (false)
      {}

///////////////////////////////////////////////////////////////////////////////////////////////////
///Создание новой очереди
///////////////////////////////////////////////////////////////////////////////////////////////////
    static Pointer Create ()
    {
      return Pointer (new ActionQueue, false);
    }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Регистрация обработчиков
///////////////////////////////////////////////////////////////////////////////////////////////////
    xtl::connection RegisterEventHandler (float first_fire, float period, const EventHandler& handler)
    {
      if (!paused)
        RegisterIdleHandler ();

      return signal.connect (EventHandlerWrapper (first_fire, period, handler));
    }

    xtl::connection RegisterEventHandler (const EventHandler& handler)
    {
      if (!paused)
        RegisterIdleHandler ();

      return signal.connect (EventHandlerWrapper (0, 0, handler));
    }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Пауза / возобновление
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Pause ()
    {
      idle_connection.disconnect ();
      paused = true;
    }

    void Resume ()
    {
      if (!paused)
        return;

      signal (true);  //обновление времени обработчиков

      RegisterIdleHandler ();

      paused = false;
    }

    bool IsPaused () const { return paused; }

    void SetPaused (bool state)
    {
      if (state) Pause ();
      else       Resume ();
    }

  private:
    void RegisterIdleHandler ()
    {
      if (idle_connection.connected ())
        return;

      idle_connection = syslib::Application::RegisterEventHandler (syslib::ApplicationEvent_OnIdle, xtl::bind (&ActionQueue::IdleHandler, this));
    }

    void IdleHandler ()
    {
      if (signal.empty ())
      {
        idle_connection.disconnect ();
        return;
      }

      signal (false);
    }

  private:
    struct EventHandlerWrapper
    {
      public:
        typedef ActionQueue::EventHandler EventHandler;

        EventHandlerWrapper (float in_first_fire, float in_period, const EventHandler& in_handler)
          : handler (in_handler)
        {
          first_fire       = size_t (in_first_fire * 1000.f);
          period           = size_t (in_period * 1000.f);
          accumulated_time = 0;
          previous_time    = common::milliseconds ();
          fired            = false;
        }

        void operator () (bool update_after_pause)
        {
          size_t current_time = common::milliseconds ();

          if (!update_after_pause)
            accumulated_time += current_time - previous_time;

          previous_time = current_time;

          if (!fired)
          {
            if (accumulated_time < first_fire)
              return;

            CallHandler ();

            fired = true;

            return;
          }

          if (accumulated_time < period)
            return;

          CallHandler ();
        }

        void CallHandler ()
        {
          try
          {
            handler (accumulated_time / 1000.f);
          }
          catch (xtl::exception& exception)
          {
            try
            {
              common::Log (LOG_NAME).Printf ("Exception in action queue handler: '%s'", exception.what ());
            }
            catch (...)
            {
            }
          }
          catch (...)
          {
            try
            {
              common::Log (LOG_NAME).Print ("Unknown exception in action queue handler");
            }
            catch (...)
            {
            }
          }

          accumulated_time = 0;
        }

      private:
        size_t       previous_time;    //время предыдущего вызова обработчика
        size_t       accumulated_time; //время с предыдущего срабатывания обработчика
        size_t       first_fire;
        size_t       period;
        bool         fired;            //true если обработчик вызывался
        EventHandler handler;
    };

  private:
    typedef xtl::signal<void (bool)> ActionQueueSignal;

  private:
    ActionQueueSignal    signal;          //сигнал обработки таймеров
    xtl::auto_connection idle_connection; //соединение idle
    bool                 paused;          //находится ли в режиме паузы
};

}

namespace engine
{

void bind_common_action_queue (script::Environment& environment)
{
  InvokerRegistry lib = environment.CreateLibrary (COMMON_ACTION_QUEUE_LIBRARY);

    //регистрация функций создания

  lib.Register ("Create", make_invoker (&ActionQueue::Create));

    //регистрация операций

  lib.Register ("CreateEventHandler",   make_callback_invoker<ActionQueue::EventHandler::signature_type> ());
  lib.Register ("RegisterEventHandler", make_invoker (make_invoker (xtl::implicit_cast<xtl::connection (ActionQueue::*) (float, float, const ActionQueue::EventHandler&)> (&ActionQueue::RegisterEventHandler)),
                                                      make_invoker (xtl::implicit_cast<xtl::connection (ActionQueue::*) (const ActionQueue::EventHandler&)> (&ActionQueue::RegisterEventHandler))));
  lib.Register ("Pause",                make_invoker (&ActionQueue::Pause));
  lib.Register ("Resume",               make_invoker (&ActionQueue::Resume));
  lib.Register ("get_Paused",           make_invoker (&ActionQueue::IsPaused));
  lib.Register ("set_Paused",           make_invoker (&ActionQueue::SetPaused));

    //регистрация типов данных

  environment.RegisterType<ActionQueue> (COMMON_ACTION_QUEUE_LIBRARY);
}

}
