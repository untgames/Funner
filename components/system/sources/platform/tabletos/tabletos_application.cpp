#include "shared.h"

using namespace syslib;
using namespace syslib::tabletos;

namespace
{

/*
    Константы
*/

const char* LOG_NAME = "system.tabletos"; //имя потока отладочного протоколирования

/*
    Менеджер обработки событий приложения
*/

class TabletOsEventListener: public MessageQueue::Handler
{
  public:
    virtual void OnSystemEvent () = 0;
  
  protected:
    virtual ~TabletOsEventListener () {}
};

class TabletOsEventManager
{
  public:
///Конструктор
    TabletOsEventManager (TabletOsEventListener& in_event_listener)
      : log (LOG_NAME)
      , event_channel_id ()
      , message_queue (*MessageQueueSingleton::Instance ())              
      , event (new Event (in_event_listener), false)
      , event_listener (in_event_listener)
      , stopping ()
    {
      try
      {      
        if (bps_channel_create (&event_channel_id, 0))
          raise_error ("::bps_channel_create");
        
        thread.reset (new Thread (xtl::bind (&TabletOsEventManager::EventProcessingRoutine, this)));
      }
      catch (xtl::exception& e)
      {
        e.touch ("syslib::tabletos::TabletOsEventManager::TabletOsEventManager");
        throw;
      }
    }
  
///Деструктор
    ~TabletOsEventManager ()
    {
      try
      {        
        stopping = true;
        
        thread->Join ();
        
        bps_channel_destroy (event_channel_id);        
      }
      catch (...)
      {
      }
    }
    
///Получение Event channel ID
    int EventChannelId () { return event_channel_id; }    
    
  private:
///Нить обработки событий
    int EventProcessingRoutine ()
    {
      log.Printf ("Enter to TabletOS event processing loop");
      
        //обработка системных событий
      
      while (!stopping)
      {
        try
        {        
          static const int timeout_ms = 1000;
          
          bps_event_t *event = 0;

          if (bps_get_event (&event, timeout_ms) != BPS_SUCCESS)
            raise_error ("::bps_get_event");                      

          if (!event)
            continue;
            
          if (bps_channel_push_event (event_channel_id, event))
            raise_error ("::bps_channel_push_event");
                      
          message_queue.PushMessage (event_listener, this->event);
        }
        catch (std::exception& e)
        {
          log.Printf ("%s\n    at syslib::tabletos::TabletOsEventManager::EventProcessingRoutine", e.what ());
        }
        catch (...)
        {
          log.Printf ("unknown exception\n    at syslib::tabletos::TabletOsEventManager::EventProcessingRoutine");          
        }        
      }      
      
      log.Printf ("Exit from TabletOS event processing loop");
      
      return 0;
    }
    
  private:
    struct Event: public MessageQueue::Message
    {
      public:
        Event (TabletOsEventListener& in_listener) : listener (in_listener) {}
      
        void Dispatch ()
        {
          listener.OnSystemEvent ();
        }
        
      private:
        TabletOsEventListener& listener;
    };
    
  private:
    common::Log              log;              //поток отладочного протоколирования
    stl::auto_ptr<Thread>    thread;           //нить обработки событий
    int                      event_channel_id; //канал обработки событий
    MessageQueue&            message_queue;    //очередь сообщений
    MessageQueue::MessagePtr event;            //событие
    TabletOsEventListener&   event_listener;   //слушатель событий
    volatile bool            stopping;         //флаг остановки нити обработки событий
};

/*
    Делегат для TabletOs приложения
*/

class TabletOsApplicationDelegate: public IApplicationDelegate, public xtl::reference_counter, public TabletOsEventListener
{
  public:
///Конструктор
    TabletOsApplicationDelegate ()
      : message_queue (*MessageQueueSingleton::Instance ())        
    {
      try
      {
        idle_enabled = false;
        is_exited    = false;
        listener     = 0;
        
        MessageQueueSingleton::Instance ()->RegisterHandler (*this);
      }
      catch (xtl::exception& e)
      {
        e.touch ("syslib::tabletos::TabletOsApplicationDelegate::TabletOsApplicationDelegate");
        throw;
      }
    }
    
///Деструктор
    ~TabletOsApplicationDelegate ()
    {
      try
      {        
        MessageQueueSingleton::Instance ()->UnregisterHandler (*this);
      }
      catch (...)
      {
        //подавление всех исключений
      }      
    }
    
///Запуск цикла обработки сообщений
    void Run ()
    {      
      platform_initialize ();      
      
      if (navigator_request_events (0) != BPS_SUCCESS)
        raise_error ("::navigator_request_events");             
        
      TabletOsEventManager event_manager (*this);
      
      int prev_channel = bps_channel_set_active (event_manager.EventChannelId ());
      
      if (prev_channel == BPS_FAILURE)
        raise_error ("::bps_channel_set_active");
      
      try
      {
        if (listener)
          listener->OnInitialize ();
        
        while (!is_exited)
        {        
          while (!IsMessageQueueEmpty ())
            DoNextEvent ();                              

           //обработка внутренней очереди сообщений

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
      catch (...)
      {
        bps_channel_set_active (prev_channel);
        throw;
      }
    }
    
///Выход из приложения
    void Exit (int code)
    {
      message_queue.PushMessage (*this, MessageQueue::MessagePtr (new ExitMessage (*this, code), false));      
    }
    
///Обработка события выхода из приложения
    void OnExit (int code)
    {
      is_exited = true;
      
      if (listener)
        listener->OnExit (code);
    }        

///Установка необходимости вызова событий idle
    void SetIdleState (bool state)
    {
      idle_enabled = state;
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
///Отложенное события выхода из приложения
    struct ExitMessage: public MessageQueue::Message
    {
      ExitMessage (TabletOsApplicationDelegate& in_delegate, int in_code) : delegate (in_delegate), code (in_code) {}
      
      void Dispatch ()
      {
        delegate.OnExit (code);
      }
      
      TabletOsApplicationDelegate& delegate;
      int                          code;
    };

///Проверка очереди событий на пустоту
    bool IsMessageQueueEmpty ()
    {
      if (is_exited)
        return true;
        
      return message_queue.IsEmpty ();
    }    
    
///Ожидание события
    void WaitMessage ()
    {
      try
      {
        message_queue.WaitMessage ();
      }
      catch (xtl::exception& exception)
      {
        exception.touch ("syslib::tabletos::TabletOsApplicationDelegate::WaitMessage");
        throw;
      }
    }
    
///Обработка следующего события
    void DoNextEvent ()
    {
      try
      {
        message_queue.DispatchFirstMessage ();
      }
      catch (xtl::exception& exception)
      {
        exception.touch ("syslib::tabletos::TabletOsApplicationDelegate::DoNextEvent");
        throw;
      }
    }            
    
///Обработка системного события
    void OnSystemEvent ()
    {
      try
      {
        static const int timeout_ms = 0;
        
        bps_event_t *event = 0;

        if (bps_get_event (&event, timeout_ms) != BPS_SUCCESS)
          raise_error ("::bps_get_event");                      

        if (!event)
          return;        
        
        int domain = bps_event_get_domain (event);

        if (domain == screen_get_domain())
        {
          printf ("screen_domain = %d\n", domain); fflush (stdout);
          HandleScreenEvent (event);
        }
        else if (domain == navigator_get_domain())
        {
          printf ("navigator_domain = %d\n", domain); fflush (stdout);
          HandleNavigatorEvent (event);
        }
      }
      catch (xtl::exception& e)
      {
        e.touch ("TabletOsApplicationDelegate::OnSystemEvent");
        throw;
      }
    }
  
///Обработка событий экрана
    void HandleScreenEvent (bps_event_t *event)
    {
      screen_event_t screen_event = screen_event_get_event (event);

      int event_type;
      screen_get_event_property_iv(screen_event, SCREEN_PROPERTY_TYPE, &event_type);

      switch (event_type)
      {
        case SCREEN_EVENT_CREATE:         // Dispatched when a child window is created.
        case SCREEN_EVENT_CLOSE:          // Dispatched when a child window is destroyed.
        case SCREEN_EVENT_MTOUCH_TOUCH:   // Dispatched when a multi-touch event is detected.
        case SCREEN_EVENT_MTOUCH_MOVE:    // Dispatched when a multi-touch move event is detected, for example when the user moves his or her fingers to make an input gesture. 
        case SCREEN_EVENT_MTOUCH_RELEASE: // Dispatched when a multi-touch release event occurs, or when the user completes the multi-touch gesture.
        case SCREEN_EVENT_POINTER:        // Dispatched when a pointer input event occurs.
        {
          screen_window_t window_handle;
          screen_get_event_property_pv (screen_event, SCREEN_PROPERTY_WINDOW, (void**)&window_handle);

          IWindowImpl* window = WindowRegistry::FindWindow (window_handle);
          
          printf ("found window = %p\n", window); fflush (stdout);

          if (!window)
            break;

          window->OnWindowEvent (event_type, screen_event);
          
          break;
        }
        
        case SCREEN_EVENT_INPUT:    // Dispatched when an unknown input event occurs.
        case SCREEN_EVENT_JOG:      // Dispatched when an jog dial input event occurs. 
        case SCREEN_EVENT_KEYBOARD: // Dispatched when a keyboard input event occurs.
          break;
                
        case SCREEN_EVENT_NONE:            // A blocking event indicating that there are currently no events in the queue. 
        case SCREEN_EVENT_PROPERTY:        // Dispatched when a property is set.
        case SCREEN_EVENT_USER:            // Dispatched when a user event is detected. 
        case SCREEN_EVENT_POST:            // Dispatched when a child window has posted its first frame.
        case SCREEN_EVENT_EFFECT_COMPLETE: // Dispatched to the window manager indicating that a rotation effect has completed.
        case SCREEN_EVENT_DISPLAY:         // Dispatched when an external display is detected.
        case SCREEN_EVENT_IDLE:            // Dispatched when the window enters idle state.
//        case SCREEN_EVENT_UNREALIZE:       // Dispatched when a handle to a window is lost.
          break;
        
        default:
          break;
      }
    }
    
///Обработка событий навигатора
    void HandleNavigatorEvent (bps_event_t *event)
    {
      unsigned int code = bps_event_get_code (event);
      
      switch (code)  
      {
        case NAVIGATOR_INVOKE:
        case NAVIGATOR_EXIT:
        case NAVIGATOR_WINDOW_STATE:
        case NAVIGATOR_SWIPE_DOWN:
        case NAVIGATOR_SWIPE_START:
        case NAVIGATOR_LOW_MEMORY:
        case NAVIGATOR_ORIENTATION_CHECK:
        case NAVIGATOR_ORIENTATION:
        case NAVIGATOR_BACK:
        case NAVIGATOR_WINDOW_ACTIVE:
        case NAVIGATOR_WINDOW_INACTIVE:
//        case NAVIGATOR_ORIENTATION_RESULT:
        default:
          break;
      }
    }

  private:
    bool                  idle_enabled;
    bool                  is_exited;
    IApplicationListener* listener;
    MessageQueue&         message_queue;        
};

}

/*
    Создание делегата приложения
*/

IApplicationDelegate* TabletOsApplicationManager::CreateDefaultApplicationDelegate ()
{
  return new TabletOsApplicationDelegate;
}

/*
    Открытие URL во внешнем браузере
*/

void TabletOsApplicationManager::OpenUrl (const char* url)
{
  throw xtl::make_not_implemented_exception ("syslib::TabletOsApplicationManager::OpenUrl");
}
