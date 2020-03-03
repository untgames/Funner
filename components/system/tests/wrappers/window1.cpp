#include "shared.h"

typedef xtl::shared_ptr<Window> WindowPtr;

WindowPtr window_holder;

const char* get_event_name (WindowEvent event)
{
  switch (event)
  {
    case WindowEvent_OnLeftButtonDown:          return "left button down";
    case WindowEvent_OnLeftButtonUp:            return "left button up";
    case WindowEvent_OnLeftButtonDoubleClick:   return "left button double click";
    case WindowEvent_OnRightButtonDown:         return "right button down";
    case WindowEvent_OnRightButtonUp:           return "right button up";
    case WindowEvent_OnRightButtonDoubleClick:  return "right button double click";
    case WindowEvent_OnMiddleButtonDown:        return "middle button down";
    case WindowEvent_OnMiddleButtonUp:          return "middle button up";
    case WindowEvent_OnMiddleButtonDoubleClick: return "middle button double click";
    case WindowEvent_OnXButton1Down:            return "XButton1 down";
    case WindowEvent_OnXButton1Up:              return "XButton1 up";
    case WindowEvent_OnXButton1DoubleClick:     return "XButton1 double click";
    case WindowEvent_OnXButton2Down:            return "XButton2 down";
    case WindowEvent_OnXButton2Up:              return "XButton2 up";
    case WindowEvent_OnXButton2DoubleClick:     return "XButton2 double click";
    default:                                    return "Unknown";
  }
}

void keys (Window& window, WindowEvent event, const WindowEventContext& context)
{
  printf ("window '%s' pressed keys: alt-%c control-%c shift-%c; pressed mouse keys: 1-%c 2-%c 3-%c; event: ",
          window.Title (), context.keyboard_alt_pressed ? 'y' : 'n', context.keyboard_control_pressed ? 'y' : 'n',
          context.keyboard_shift_pressed ? 'y' : 'n', context.mouse_left_button_pressed ? 'y' : 'n',
          context.mouse_right_button_pressed ? 'y' : 'n', context.mouse_middle_button_pressed ? 'y' : 'n');

  switch (event)
  {
    case WindowEvent_OnKeyDown:
      printf ("key '%s' scan '%s' down\n", get_key_name (context.key), get_key_scan_name (context.key_scan_code));
      break;
    case WindowEvent_OnKeyUp:
      printf ("key '%s' scan '%s' up\n", get_key_name (context.key), get_key_scan_name (context.key_scan_code));
      break;
    case WindowEvent_OnLeftButtonDown:
    case WindowEvent_OnLeftButtonUp:
    case WindowEvent_OnLeftButtonDoubleClick:
    case WindowEvent_OnRightButtonDown:
    case WindowEvent_OnRightButtonUp:
    case WindowEvent_OnRightButtonDoubleClick:
    case WindowEvent_OnMiddleButtonDown:
    case WindowEvent_OnMiddleButtonUp:
    case WindowEvent_OnMiddleButtonDoubleClick:
    case WindowEvent_OnXButton1Down:
    case WindowEvent_OnXButton1Up:
    case WindowEvent_OnXButton1DoubleClick:
    case WindowEvent_OnXButton2Down:
    case WindowEvent_OnXButton2Up:
    case WindowEvent_OnXButton2DoubleClick:
      printf ("%s x=%lu y=%lu\n", get_event_name (event), context.cursor_position.x, context.cursor_position.y);
      
      if (event == WindowEvent_OnLeftButtonDoubleClick)
        window.SetStyle (window.Style () == WindowStyle_Overlapped ? WindowStyle_PopUp : WindowStyle_Overlapped);
      
      break;
    case WindowEvent_OnChar:
      printf ("char '%C'\n", context.char_code);
      break;
    case WindowEvent_OnMouseVerticalWheel:
      printf ("vertical wheel %.2f (x=%u, y=%u)\n", context.mouse_vertical_wheel_delta,
        context.cursor_position.x, context.cursor_position.y);
      break;
    case WindowEvent_OnMouseHorisontalWheel:
      printf ("horizontal wheel %.2f (x=%u, y=%u)\n", context.mouse_horisontal_wheel_delta,
        context.cursor_position.x, context.cursor_position.y);      
      break;
    default:
      break;
  }
  
  fflush (stdout);  
}

void mousemove (Window& window, WindowEvent, const WindowEventContext& context)
{
  printf ("window '%s': mouse move x=%lu y=%lu\n", window.Title (), context.cursor_position.x, context.cursor_position.y);
  fflush (stdout);  
}

void mouseleave (Window& window, WindowEvent, const WindowEventContext& context)
{
  printf ("window '%s': mouse leave x=%lu y=%lu\n", window.Title (), context.cursor_position.x, context.cursor_position.y);
  fflush (stdout);
}

void touch_event (Window& window, WindowEvent event, const WindowEventContext& context)
{
  printf ("window '%s': touch event ", window.Title ());

  switch (event)
  {
    case WindowEvent_OnTouchesBegan:
      printf ("TouchesBegan");
      break;
    case WindowEvent_OnTouchesMoved:
      printf ("TouchesMoved");
      break;
    case WindowEvent_OnTouchesDoubletap:
      printf ("TouchesDoubletap");
      break;
    case WindowEvent_OnTouchesEnded:
      printf ("TouchesEnded");
      break;
    default:
      printf ("Unknown");
      break;
  }

  printf (". %u touches in event:\n", context.touches_count);

  for (size_t i = 0; i < context.touches_count; i++)
  {
    const Touch& touch = context.touches [i];

    printf ("  touch %u position %u %u\n", touch.touch_id, touch.position.x, touch.position.y);
  }

  fflush (stdout);
}

void destroy (Window& window, WindowEvent, const WindowEventContext&)
{
  printf ("window '%s': destroyed\n", window.Title ());
  fflush (stdout);  

  throw std::bad_alloc (); //тестирование распространения исключений в обработчиках событий
}

void print_event (Window& window, WindowEvent event, const WindowEventContext& context)
{
  switch (event)
  {
    case WindowEvent_OnClose:
      printf ("Window close event\n");
      Application::Exit (0);
      
      break;
    case WindowEvent_OnChangeHandle: printf ("Window changed handle to %p event\n", context.handle); break;
    case WindowEvent_OnActivate:     printf ("Window activate event\n");       break;
    case WindowEvent_OnDeactivate:   printf ("Window deactivate event\n");     break;
    case WindowEvent_OnShow:         printf ("Window show event\n");           break;
    case WindowEvent_OnHide:         printf ("Window hide event\n");           break;
    case WindowEvent_OnSetFocus:     printf ("Window set focus event\n");      break;
    case WindowEvent_OnLostFocus:    printf ("Window lost focus event\n");     break;
    case WindowEvent_OnPaint:        printf ("Window paint event\n");          break;
    case WindowEvent_OnMove:         printf ("Window move event\n");           break;
    case WindowEvent_OnSize:         printf ("Window size event\n");           break;
    case WindowEvent_OnScreenLock:   printf ("Window screen lock event\n");    break;
    case WindowEvent_OnScreenUnlock: printf ("Window screen unlock event\n");  break;
    default: return;
  }
  
  fflush (stdout);
}

void app_exit ()
{
  printf ("application exit\n");
  fflush (stdout);
}

void print (const char* message)
{
  printf ("window debug: %s\n", message);
  fflush (stdout);  
}

void log_print (const char* stream, const char* message)
{
  printf ("%s: %s\n", stream, message);
  fflush (stdout);
}

void on_application_initialized ()
{
  window_holder.reset (new Window (WindowStyle_Overlapped, 400, 300));

  Window& window = *window_holder;

  window.SetBackgroundState (true);

  window.SetTitle ("Test window");
  window.Show ();
  window.SetFocus ();

  window.RegisterEventHandler (WindowEvent_OnKeyDown, &keys);
  window.RegisterEventHandler (WindowEvent_OnKeyUp, &keys);
  window.RegisterEventHandler (WindowEvent_OnMouseMove, &mousemove);
  window.RegisterEventHandler (WindowEvent_OnMouseLeave, &mouseleave);
  window.RegisterEventHandler (WindowEvent_OnDestroy, &destroy);
  window.RegisterEventHandler (WindowEvent_OnLeftButtonDown, &keys);
  window.RegisterEventHandler (WindowEvent_OnLeftButtonUp, &keys);
  window.RegisterEventHandler (WindowEvent_OnLeftButtonDoubleClick, &keys);
  window.RegisterEventHandler (WindowEvent_OnRightButtonDown, &keys);
  window.RegisterEventHandler (WindowEvent_OnRightButtonUp, &keys);
  window.RegisterEventHandler (WindowEvent_OnRightButtonDoubleClick, &keys);
  window.RegisterEventHandler (WindowEvent_OnMiddleButtonDown, &keys);
  window.RegisterEventHandler (WindowEvent_OnMiddleButtonUp, &keys);
  window.RegisterEventHandler (WindowEvent_OnMiddleButtonDoubleClick, &keys);
  window.RegisterEventHandler (WindowEvent_OnChar, &keys);
  window.RegisterEventHandler (WindowEvent_OnMouseVerticalWheel, &keys);
  window.RegisterEventHandler (WindowEvent_OnMouseHorisontalWheel, &keys);
  window.RegisterEventHandler (WindowEvent_OnXButton1Down, &keys);
  window.RegisterEventHandler (WindowEvent_OnXButton1Up, &keys);
  window.RegisterEventHandler (WindowEvent_OnXButton1DoubleClick, &keys);
  window.RegisterEventHandler (WindowEvent_OnXButton2Down, &keys);
  window.RegisterEventHandler (WindowEvent_OnXButton2Up, &keys);
  window.RegisterEventHandler (WindowEvent_OnXButton2DoubleClick, &keys);
  window.RegisterEventHandler (WindowEvent_OnClose, &print_event);
  window.RegisterEventHandler (WindowEvent_OnActivate, &print_event);
  window.RegisterEventHandler (WindowEvent_OnDeactivate, &print_event);
  window.RegisterEventHandler (WindowEvent_OnShow, &print_event);
  window.RegisterEventHandler (WindowEvent_OnHide, &print_event);
  window.RegisterEventHandler (WindowEvent_OnSetFocus, &print_event);
  window.RegisterEventHandler (WindowEvent_OnLostFocus, &print_event);
  window.RegisterEventHandler (WindowEvent_OnPaint, &print_event);
  window.RegisterEventHandler (WindowEvent_OnMove, &print_event);
  window.RegisterEventHandler (WindowEvent_OnSize, &print_event);
  window.RegisterEventHandler (WindowEvent_OnChangeHandle, &print_event);
  window.RegisterEventHandler (WindowEvent_OnTouchesBegan, &touch_event);
  window.RegisterEventHandler (WindowEvent_OnTouchesDoubletap, &touch_event);
  window.RegisterEventHandler (WindowEvent_OnTouchesMoved, &touch_event);
  window.RegisterEventHandler (WindowEvent_OnTouchesEnded, &touch_event);
  window.RegisterEventHandler (WindowEvent_OnScreenLock, &print_event);
  window.RegisterEventHandler (WindowEvent_OnScreenUnlock, &print_event);

  window.SetDebugLog (&print);
  window.SetMultitouchEnabled (true);

  fflush (stdout);
}

int main ()
{
  printf ("Results of window1_test:\n");

  try
  {
    common::LogFilter log_filter ("*", &log_print);
    
    Application::RegisterEventHandler (ApplicationEvent_OnInitialize, &on_application_initialized);
    Application::RegisterEventHandler (ApplicationEvent_OnExit, &app_exit),

    Application::Run ();

    return Application::GetExitCode ();
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
