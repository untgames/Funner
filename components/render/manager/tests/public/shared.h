#ifndef RENDER_MANAGER_TESTS_SHARED_HEADER
#define RENDER_MANAGER_TESTS_SHARED_HEADER

#include <cstdio>
#include <exception>

#include <xtl/bind.h>
#include <xtl/function.h>
#include <xtl/ref.h>
#include <xtl/trackable.h>

#include <common/log.h>
#include <common/time.h>

#include <render/manager.h>

#include <math/utility.h>

#include <media/image.h>
#include <media/rfx/material_library.h>

#include <syslib/application.h>
#include <syslib/window.h>

using namespace common;
using namespace render::manager;

//ъюэёЄрэЄ√
const size_t WINDOW_WIDTH  = 800;
const size_t WINDOW_HEIGHT = 600;

static void log_print (const char* log_name, const char* message)
{
  printf ("%s: %s\n", log_name, message);
  fflush (stdout);
}

//яЁюЄюъюы ЄхёЄр
struct TestLogFilter
{
  stl::auto_ptr<common::LogFilter> log_filter;

  TestLogFilter (bool logging)
  {
    if (logging)
      log_filter = stl::auto_ptr<common::LogFilter> (new common::LogFilter ("render.manager*", &log_print));
  }
};

///ТхёЄютюх яЁшыюцхэшх
class Test: private xtl::trackable, private TestLogFilter
{
  public:
///КюэёЄЁєъЄюЁ
    Test (const wchar_t* title, bool logging = true)
      : TestLogFilter (logging)
      , window (syslib::WindowStyle_Overlapped, WINDOW_WIDTH, WINDOW_HEIGHT)
      , render_window (CreateRenderWindow ())
    {    
      window.SetTitle (title);
      
      connect_tracker (window.RegisterEventHandler (syslib::WindowEvent_OnClose, xtl::bind (&Test::OnWindowClose, this)));
    }
    
///Пюыєўхэшх ьхэхфцхЁр ЁхэфхЁшэур
    render::manager::RenderManager RenderManager ()
    {
      return render_manager;
    }
    
///Пюыєўхэшх юъэр
    render::manager::Window Window ()
    {
      return render_window;
    }
    
///Пюърч юъэр
    void ShowWindow ()
    {
      window.Show ();
    }    
    
///Гыртэ√щ Ўшъы
    int Run ()
    {
      window.Show ();    
    
      syslib::Application::Run ();

      return syslib::Application::GetExitCode ();
    }
    
///ТхёЄ шчьхэхэш▀ ёЄшы▀ юъэр
    void TestChangeWindowStyle ()
    {
      window.SetStyle (syslib::WindowStyle_PopUp);
      window.SetStyle (syslib::WindowStyle_Overlapped);
    }
    
  private:
///Сючфрэшх юъэр ЁхэфхЁшэур
    render::manager::Window CreateRenderWindow ()
    {
      connect_tracker (render_manager.RegisterWindowEventHandler (RenderManagerWindowEvent_OnAdded, xtl::bind (&Test::OnWindowAdded, this, _2)));
      connect_tracker (render_manager.RegisterWindowEventHandler (RenderManagerWindowEvent_OnRemoved, xtl::bind (&Test::OnWindowRemoved, this, _2)));

      PropertyMap properties;
      
      properties.SetProperty ("ColorBits", 24);
      properties.SetProperty ("DepthBits", 16);
    
      return render_manager.CreateWindow (window, properties);
    }
    
///ОсЁрсюЄър ёюс√Єш▀ фюсртыхэш▀ юъэр
    void OnWindowAdded (render::manager::Window& window)
    {
      printf ("window added\n");
      
      connect_tracker (window.RegisterEventHandler (WindowEvent_OnResize, xtl::bind (&Test::OnWindowResize, this, _1)));
    }
    
///ОсЁрсюЄър ёюс√Єш▀ єфрыхэш▀ юъэр
    void OnWindowRemoved (render::manager::Window& window)
    {
      printf ("window removed\n");
    }
    
///ОсЁрсюЄър ёюс√Єш▀ шчьхэхэш▀ ЁрчьхЁют юъэр
    void OnWindowResize (render::manager::Window& window)
    {
      printf ("window resize\n");
    }
    
///ОсЁрсюЄър ёюс√Єш▀ чръЁ√Єш▀ юъэр
    void OnWindowClose ()
    {
      syslib::Application::Exit (0);
    }
    
///ОсЁрсюЄър
  
  private:
    syslib::Window                  window;
    render::manager::RenderManager  render_manager;
    render::manager::Window         render_window;
};

#endif
