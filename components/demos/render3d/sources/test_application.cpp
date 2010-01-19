#include "shared.h"

namespace
{

#ifdef _MSC_VER
  #pragma warning (disable : 4355) //'this' : used in base member initializer list
#endif

const float HORIZONTAL_SPEED          = 2.0f;
const float VERTICAL_SPEED            = HORIZONTAL_SPEED;
const float HORIZONTAL_ROTATION_SPEED = 30.0f;
const float VERTICAL_ROTATION_SPEED   = HORIZONTAL_ROTATION_SPEED;
const float FOV_X_ASPECT_RATIO        = 90.f;

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

}

Test::Test (const wchar_t* title, const CallbackFn& in_redraw, const CallbackFn& in_reload, const char* adapter_mask, const char* init_string)
  : window (syslib::WindowStyle_Overlapped, 800, 600),
    redraw (in_redraw),
    reload (in_reload),
    shader_manager (*this),
    scene_renderer (*this),
    mesh_manager (*this),
    material_manager (*this),
    x_camera_speed (0),
    y_camera_speed (0),
    x_camera_rotation_speed (0),
    y_camera_rotation_speed (0)
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

  camera = scene_graph::PerspectiveCamera::Create ();

  camera->SetZNear (1);
  camera->SetZFar  (100);

  camera->SetPosition (0, 0, -10);

  camera->BindToScene (scene_manager.Scene ());

  light = scene_graph::DirectLight::Create ();

  light->BindToParent (*camera);

//    swap_chain->SetFullscreenState (true);

  memset (pressed_keys, 0, sizeof (pressed_keys));

  OnResize ();

  window.RegisterEventHandler (syslib::WindowEvent_OnPaint, xtl::bind (&Test::OnRedraw, this));
  window.RegisterEventHandler (syslib::WindowEvent_OnSize, xtl::bind (&Test::OnResize, this));
  window.RegisterEventHandler (syslib::WindowEvent_OnClose, xtl::bind (&Test::OnClose, this));
  window.RegisterEventHandler (syslib::WindowEvent_OnKeyDown, xtl::bind (&Test::OnKeyPressed, this, _3));
  window.RegisterEventHandler (syslib::WindowEvent_OnKeyUp, xtl::bind (&Test::OnKeyReleased, this, _3));

  window.Invalidate ();
}

void Test::OnKeyPressed (const syslib::WindowEventContext& context)
{
  if (pressed_keys [context.key])
    return;

  pressed_keys [context.key] = true;

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
    case syslib::Key_A:
      x_camera_speed -= HORIZONTAL_SPEED;
      break;
    case syslib::Key_D:
      x_camera_speed += HORIZONTAL_SPEED;
      break;
    case syslib::Key_W:
      y_camera_speed += VERTICAL_SPEED;
      break;
    case syslib::Key_S:
      y_camera_speed -= VERTICAL_SPEED;
      break;
    case syslib::Key_Left:
      x_camera_rotation_speed -= HORIZONTAL_ROTATION_SPEED;
      break;
    case syslib::Key_Right:
      x_camera_rotation_speed += HORIZONTAL_ROTATION_SPEED;
      break;
    case syslib::Key_Up:
      y_camera_rotation_speed -= VERTICAL_ROTATION_SPEED;
      break;
    case syslib::Key_Down:
      y_camera_rotation_speed += VERTICAL_ROTATION_SPEED;
      break;
    default:
      break;
  }
}

void Test::OnKeyReleased (const syslib::WindowEventContext& context)
{
  pressed_keys [context.key] = false;

  switch (context.key)
  {
    case syslib::Key_A:
      x_camera_speed += HORIZONTAL_SPEED;
      break;
    case syslib::Key_D:
      x_camera_speed -= HORIZONTAL_SPEED;
      break;
    case syslib::Key_W:
      y_camera_speed -= VERTICAL_SPEED;
      break;
    case syslib::Key_S:
      y_camera_speed += VERTICAL_SPEED;
      break;
    case syslib::Key_Left:
      x_camera_rotation_speed += HORIZONTAL_ROTATION_SPEED;
      break;
    case syslib::Key_Right:
      x_camera_rotation_speed -= HORIZONTAL_ROTATION_SPEED;
      break;
    case syslib::Key_Up:
      y_camera_rotation_speed += VERTICAL_ROTATION_SPEED;
      break;
    case syslib::Key_Down:
      y_camera_rotation_speed -= VERTICAL_ROTATION_SPEED;
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

    float width  = (float)(rect.right - rect.left),
          height = (float)(rect.bottom - rect.top),
          ar     = width / height;

    Viewport vp;

    vp.x         = rect.left;
    vp.y         = rect.top;
    vp.width     = (int)width;
    vp.height    = (int)height;
    vp.min_depth = 0;
    vp.max_depth = 1;

    device->RSSetViewport (vp);

    camera->SetFovX   (math::degree (FOV_X_ASPECT_RATIO));
    camera->SetFovY   (math::degree (FOV_X_ASPECT_RATIO / ar));
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
