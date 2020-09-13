#include "shared.h"

struct Test
{
  RenderTarget&          target;
  scene_graph::Sprite&   sprite;
  scene_graph::Screen&   screen;
  scene_graph::Viewport& viewport_offscreen;
  scene_graph::Viewport& viewport_onscreen;

  Test (RenderTarget& in_target, scene_graph::Sprite& in_sprite, scene_graph::Screen& in_screen,
        scene_graph::Viewport& in_viewport_offscreen, scene_graph::Viewport& in_viewport_onscreen)
    : target (in_target)
    , sprite (in_sprite)
    , screen (in_screen)
    , viewport_offscreen (in_viewport_offscreen)
    , viewport_onscreen (in_viewport_onscreen)
  {
  }
};

void log_print (const char* log_name, const char* message)
{
  printf ("%s: %s\n", log_name, message);
  fflush (stdout);
}

void on_window_close ()
{
  syslib::Application::Exit (0);
}

void idle (Test& test)
{
  try
  {
    static size_t last_fps = 0;
    static size_t frames_count = 0;
    static bool render_offscreen = true;

    if (common::milliseconds () - last_fps > 1000)
    {
      printf ("FPS: %.2f\n", float (frames_count)/float (common::milliseconds () - last_fps)*1000.f);
      fflush (stdout);

      last_fps = common::milliseconds ();
      frames_count = 0;

      render_offscreen = !render_offscreen;

      test.viewport_offscreen.SetTechnique (render_offscreen ? "offscreen" : "default");

      if (render_offscreen)
        test.screen.Attach (test.viewport_onscreen);
      else
        test.screen.Detach (test.viewport_onscreen);

      return;
    }
    
    frames_count++;
    
    float angle = common::milliseconds () / 100.0f;
    
    test.sprite.SetWorldOrientation (math::degree (angle), 0.0f, 0.0f, 1.0f);

    test.target.Update ();      
  }
  catch (std::exception& e)
  {
    printf ("exception in idle: %s\n", e.what ());
  }  
}

int main ()
{
  printf ("Results of offscreen1_test:\n");

  try
  {
    common::LogFilter log_filter ("render.*", &log_print);

    syslib::Window window (syslib::WindowStyle_Overlapped, 400, 400);

    window.RegisterEventHandler (syslib::WindowEvent_OnClose, xtl::bind (&on_window_close));

    const char* SERVER_NAME = "MyServer";

    Server server (SERVER_NAME);
    Client client (SERVER_NAME);

    common::PropertyMap window_properties;
    
    window_properties.SetProperty ("ColorBits", 32);
    window_properties.SetProperty ("DepthBits", 24);

    server.AttachWindow ("my_window", window, window_properties);

    client.LoadResource ("data/offscreen.rfx");
    client.LoadResource ("data/bottom.jpg");
    client.LoadResource ("data/offscreen.xmtl");

    RenderTarget target = client.CreateRenderTarget ("my_window");

    scene_graph::Screen screen;
    
    target.SetScreen (&screen);
    
    scene_graph::OrthoCamera::Pointer camera_offscreen = scene_graph::OrthoCamera::Create ();
    
    camera_offscreen->SetLeft   (-10.f);
    camera_offscreen->SetRight  (10.0f);
    camera_offscreen->SetBottom (-10.f);
    camera_offscreen->SetTop    (10.0f);
    camera_offscreen->SetZNear  (0.0f);
    camera_offscreen->SetZFar   (10.0f);

    camera_offscreen->SetPosition (0, 0.0f, -1.0f);

    scene_graph::OrthoCamera::Pointer camera_onscreen = scene_graph::OrthoCamera::Create ();

    camera_onscreen->SetLeft   (0.f);
    camera_onscreen->SetRight  (1.0f);
    camera_onscreen->SetBottom (0.f);
    camera_onscreen->SetTop    (1.0f);
    camera_onscreen->SetZNear  (0.0f);
    camera_onscreen->SetZFar   (10.0f);

    camera_onscreen->SetPosition (0, 0.0f, -1.0f);

    scene_graph::Scene scene_offscreen, scene_onscreen;
    
    camera_offscreen->BindToScene (scene_offscreen);
    camera_onscreen->BindToScene (scene_onscreen);
    
    scene_graph::Sprite::Pointer sprite_offscreen = scene_graph::Sprite::Create ();
    
    sprite_offscreen->SetMaterial ("offscreen_material");
    sprite_offscreen->SetScale (math::vec3f (20.f));

    sprite_offscreen->BindToScene (scene_offscreen);

    scene_graph::Sprite::Pointer sprite_onscreen = scene_graph::Sprite::Create ();

    sprite_onscreen->SetMaterial ("onscreen_material");
    sprite_onscreen->SetScale (math::vec3f (1.0f));
    sprite_onscreen->SetWorldPosition (math::vec3f (0.5f, 0.5f, 1.0f));

    sprite_onscreen->BindToScene (scene_onscreen);

      //настройка области вывода
    
    scene_graph::Viewport vp_offscreen;
    
    vp_offscreen.SetArea       (0, 0, 100, 100);
    vp_offscreen.SetCamera     (camera_offscreen.get ());
    vp_offscreen.SetTechnique  ("offscreen");
    vp_offscreen.SetZOrder     (1);
    
    screen.Attach (vp_offscreen);

    scene_graph::Viewport vp_onscreen;

    vp_onscreen.SetArea       (0, 0, 100, 100);
    vp_onscreen.SetCamera     (camera_onscreen.get ());
    vp_onscreen.SetTechnique  ("onscreen");
    vp_onscreen.SetZOrder     (0);

    screen.Attach (vp_onscreen);

    window.Show ();    

    Test test (target, *sprite_offscreen, screen, vp_offscreen, vp_onscreen);

    syslib::Application::RegisterEventHandler (syslib::ApplicationEvent_OnIdle, xtl::bind (&idle, xtl::ref (test)));
    
    syslib::Application::Run ();
  }
  catch (std::exception& e)
  {
    printf ("%s\n", e.what ());
  }
}
