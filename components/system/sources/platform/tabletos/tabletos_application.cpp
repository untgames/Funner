#include "shared.h"

using namespace syslib;
using namespace syslib::tabletos;

namespace
{

class TabletOsApplicationDelegate: public IApplicationDelegate, public xtl::reference_counter
{
  public:
///Конструктор
    TabletOsApplicationDelegate ()
    {
      idle_enabled = false;
      is_exited    = false;
      listener     = 0;
    }
    
///Запуск цикла обработки сообщений
    void Run ()
    {
      platform_initialize ();
      
      if (navigator_request_events (0) != BPS_SUCCESS)
        raise_error ("::navigator_request_events");
      
      if (listener)
        listener->OnInitialized ();
      
      while (!is_exited)
      {
        const int timeout_ms = idle_enabled && listener ? 0 : 100;
        
        bps_event_t *event = 0;

        if (bps_get_event (&event, timeout_ms) != BPS_SUCCESS)
          raise_error ("::bps_get_event");

        if (event)
        {
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

        if (idle_enabled && listener)
          listener->OnIdle ();
      }
    }

///Выход из приложения
    void Exit (int code)
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
        case NAVIGATOR_ORIENTATION_RESULT:
        default:
          break;
      }
    }

  private:
    bool                  idle_enabled;
    bool                  is_exited;
    IApplicationListener* listener;
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
