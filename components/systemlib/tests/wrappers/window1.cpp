#include "shared.h"

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
  printf ("window '%s': ", window.Title ());

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
      printf ("%s x=%d y=%d\n", get_event_name (event), context.cursor_position.x, context.cursor_position.y);
      break;
    case WindowEvent_OnChar:
      printf ("char '%C'\n", context.char_code);
      break;
    case WindowEvent_OnMouseVerticalWheel:
      printf ("vertical wheel %.2f\n", context.mouse_vertical_wheel_delta);
      break;
    case WindowEvent_OnMouseHorisontalWheel:
      printf ("horizontal wheel %.2f\n", context.mouse_horisontal_wheel_delta);
      break;
  }
}

void mousemove (Window& window, WindowEvent, const WindowEventContext& context)
{
  printf ("window '%s': mouse move x=%u y=%u\n", window.Title (), context.cursor_position.x, context.cursor_position.y);
}

void destroy (Window& window, WindowEvent, const WindowEventContext&)
{
  printf ("window '%s': destroyed\n", window.Title ());  

  Application::Exit (0);

  throw std::bad_alloc (); //тестирование распространения исключений в обработчиках событий
}

void app_exit ()
{
  printf ("application exit\n");
}

void print (const char* message)
{
  printf ("window debug: %s\n", message);
}

int main ()
{
  printf ("Results of window1_test:\n");
  
  try
  {
    Window window (WindowStyle_Overlapped, 400, 300);
    
    window.SetTitle ("Test window");
    window.Show ();

    auto_connection connection1  = window.RegisterEventHandler (WindowEvent_OnKeyDown, &keys),
                    connection2  = window.RegisterEventHandler (WindowEvent_OnKeyUp, &keys),
                    connection3  = window.RegisterEventHandler (WindowEvent_OnMouseMove, &mousemove),
                    connection4  = window.RegisterEventHandler (WindowEvent_OnDestroy, &destroy),
                    connection5  = Application::RegisterEventHandler (ApplicationEvent_OnExit, &app_exit),
                    connection6  = window.RegisterEventHandler (WindowEvent_OnLeftButtonDown, &keys),
                    connection7  = window.RegisterEventHandler (WindowEvent_OnLeftButtonUp, &keys),
                    connection8  = window.RegisterEventHandler (WindowEvent_OnLeftButtonDoubleClick, &keys),
                    connection9  = window.RegisterEventHandler (WindowEvent_OnRightButtonDown, &keys),
                    connection10 = window.RegisterEventHandler (WindowEvent_OnRightButtonUp, &keys),
                    connection11 = window.RegisterEventHandler (WindowEvent_OnRightButtonDoubleClick, &keys),
                    connection12 = window.RegisterEventHandler (WindowEvent_OnMiddleButtonDown, &keys),
                    connection13 = window.RegisterEventHandler (WindowEvent_OnMiddleButtonUp, &keys),
                    connection14 = window.RegisterEventHandler (WindowEvent_OnMiddleButtonDoubleClick, &keys),
                    connection15 = window.RegisterEventHandler (WindowEvent_OnChar, &keys),
                    connection16 = window.RegisterEventHandler (WindowEvent_OnMouseVerticalWheel, &keys),
                    connection17 = window.RegisterEventHandler (WindowEvent_OnMouseHorisontalWheel, &keys),
                    connection18 = window.RegisterEventHandler (WindowEvent_OnXButton1Down, &keys),
                    connection19 = window.RegisterEventHandler (WindowEvent_OnXButton1Up, &keys),
                    connection20 = window.RegisterEventHandler (WindowEvent_OnXButton1DoubleClick, &keys),
                    connection21 = window.RegisterEventHandler (WindowEvent_OnXButton2Down, &keys),
                    connection22 = window.RegisterEventHandler (WindowEvent_OnXButton2Up, &keys),
                    connection23 = window.RegisterEventHandler (WindowEvent_OnXButton2DoubleClick, &keys);
                    
    window.SetDebugLog (&print);        

    Application::Run ();        

    return Application::GetExitCode ();    
  }  
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
