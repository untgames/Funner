#include "shared.h"

//протокол теста
struct TestLogFilter
{
  common::LogFilter log_filter;

  TestLogFilter () : log_filter ("*", &LogPrint) {}

  static void LogPrint (const char* log_name, const char* message)
  {
    printf ("%s: %s\n", log_name, message);
    fflush (stdout);
  }
};

typedef common::Singleton<TestLogFilter> TestLogFilterSingleton;

Test::Test (const wchar_t* title, const CallbackFn& in_redraw, const CallbackFn& in_reload, const char* adapter_mask, const char* init_string)
  : window (syslib::WindowStyle_Overlapped, 800, 600),
    redraw (in_redraw),
    reload (in_reload)
{
  TestLogFilterSingleton::Instance (); //инициализация фильтра протокольных сообщений

  window.SetTitle (title);
  window.Show ();

  SwapChainDesc desc;

  memset (&desc, 0, sizeof (desc));

  desc.frame_buffer.color_bits   = 32;
  desc.frame_buffer.alpha_bits   = 0;
  desc.frame_buffer.depth_bits   = 16;
  desc.frame_buffer.stencil_bits = 0;
  desc.buffers_count             = 2;
  desc.samples_count             = 8;
  desc.swap_method               = SwapMethod_Discard;
  desc.vsync                     = false;
  desc.window_handle             = window.Handle ();

  DriverManager::CreateSwapChainAndDevice ("OpenGL", adapter_mask, desc, init_string, swap_chain, device);

//    swap_chain->SetFullscreenState (true);

  OnResize ();

  window.RegisterEventHandler (syslib::WindowEvent_OnPaint, xtl::bind (&Test::OnRedraw, this));
  window.RegisterEventHandler (syslib::WindowEvent_OnSize, xtl::bind (&Test::OnResize, this));
  window.RegisterEventHandler (syslib::WindowEvent_OnClose, xtl::bind (&Test::OnClose, this));
  window.RegisterEventHandler (syslib::WindowEvent_OnKeyDown, xtl::bind (&Test::OnKeyPressed, this, _3));

  window.Invalidate ();
}
  
void Test::OnKeyPressed (const syslib::WindowEventContext& context)
{
  switch (context.key)
  {
    case syslib::Key_F5:
      try
      {
        reload (*this);
      }
      catch (std::exception& e)
      {
        printf ("reload exception: %s\n", e.what ());
      }

      break;
    default:
      break;
  }
}

void Test::OnResize ()
{
  try
  {
    syslib::Rect rect = window.ClientRect ();

    Viewport vp;

    vp.x         = rect.left;
    vp.y         = rect.top;
    vp.width     = rect.right - rect.left;
    vp.height    = rect.bottom - rect.top;
    vp.min_depth = 0;
    vp.max_depth = 1;

    device->RSSetViewport (vp);
  }
  catch (std::exception& e)
  {
    printf ("resize exception: %s\n", e.what ());
  }
}

void Test::OnRedraw ()
{
  try
  {
    Color4f clear_color;

    clear_color.red   = 0;
    clear_color.green = 0.7f;
    clear_color.blue  = 0.7f;
    clear_color.alpha = 0;

    device->ClearViews (ClearFlag_All, clear_color, 1.0f, 0);

    if (redraw)
      redraw (*this);

    device->Flush ();

    swap_chain->Present ();
  }
  catch (std::exception& e)
  {
    printf ("redraw exception: %s\n", e.what ());
  }
}

void Test::OnClose ()
{
  syslib::Application::Exit (0);
}
