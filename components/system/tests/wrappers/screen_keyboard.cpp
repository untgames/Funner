#include "shared.h"

typedef xtl::shared_ptr<Window> WindowPtr;

WindowPtr window_holder;

void on_char (Window& window, WindowEvent event, const WindowEventContext& context)
{
  printf ("char '%s'\n", common::to_utf8_string (&context.char_code, 1).c_str ());
  fflush (stdout);  
}

void on_close (Window& window, WindowEvent event, const WindowEventContext& context)
{
  printf ("Window close event\n");
  fflush (stdout);

  Application::Exit (0);       
}

void log_print (const char* stream, const char* message)
{
  printf ("%s: %s\n", stream, message);
  fflush (stdout);
}

void on_application_initialized ()
{
  try
  {
    common::LogFilter log_filter ("*", &log_print);
    
    window_holder.reset (new Window (WindowStyle_Overlapped, 400, 300));

    Window& window = *window_holder;
    
    window.SetTitle ("Test window");
    window.Show ();
    window.SetFocus ();

    window.RegisterEventHandler (WindowEvent_OnChar, &on_char),
    window.RegisterEventHandler (WindowEvent_OnClose, &on_close);
    
    fflush (stdout);    

    ScreenKeyboard keyboard (window, ScreenKeyboardType_Ascii);

    keyboard.Show ();
//    keyboard.Hide ();
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }
}

int main ()
{
  printf ("Results of screen_keyboard_test:\n");

  try
  {
    common::LogFilter log_filter ("*", &log_print);

    Application::RegisterEventHandler (ApplicationEvent_OnInitialize, &on_application_initialized);

    Application::Run ();

    return Application::GetExitCode ();
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
