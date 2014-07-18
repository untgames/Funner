#include "shared.h"

struct Test
{
  RenderTarget&                target;
  scene_graph::Sprite::Pointer sprite;

  Test (RenderTarget& in_target, scene_graph::Sprite::Pointer in_sprite)
    : target (in_target)
    , sprite (in_sprite)
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

    if (common::milliseconds () - last_fps > 1000)
    {
      if (test.sprite)
      {
        test.sprite->Unbind ();
        test.sprite = 0;
      }

      printf ("FPS: %.2f\n", float (frames_count)/float (common::milliseconds () - last_fps)*1000.f);
      fflush (stdout);

      last_fps = common::milliseconds ();
      frames_count = 0;
      return;
    }
    
    frames_count++;
    
    float angle = common::milliseconds () / 100.0f;

    if (test.sprite)
      test.sprite->SetWorldOrientation (math::degree (angle), 0.0f, 0.0f, 1.0f);

    test.target.Update ();      
  }
  catch (std::exception& e)
  {
    printf ("exception in idle: %s\n", e.what ());
  }  
}

int main ()
{
  printf ("Results of sprite1_test:\n");

  try
  {
    common::LogFilter log_filter ("render.*", &log_print);

    syslib::Window window (syslib::WindowStyle_Overlapped, 400, 300);

    window.RegisterEventHandler (syslib::WindowEvent_OnClose, xtl::bind (&on_window_close));

    const char* SERVER_NAME = "MyServer";

    Server server (SERVER_NAME/*, render::scene::server::ServerThreadingModel_SingleThreaded*/);
    Client client (SERVER_NAME);

    common::PropertyMap window_properties;
    
    window_properties.SetProperty ("ColorBits", 32);
    window_properties.SetProperty ("DepthBits", 24);

    server.AttachWindow ("my_window", window, window_properties);

    client.LoadResource ("data/render.rfx");
    client.LoadResource ("data/bottom.jpg");
    client.LoadResource ("data/sprite.xmtl");

    RenderTarget target = client.CreateRenderTarget ("my_window");

    scene_graph::Screen screen;
    
    target.SetScreen (&screen);
    
    screen.SetBackgroundState (true);
    screen.SetBackgroundColor (math::vec4f (0.0f, 1.0f, 1.0f, 1.0f));

    scene_graph::OrthoCamera::Pointer camera = scene_graph::OrthoCamera::Create ();
    
    camera->SetLeft   (10.0f);
    camera->SetRight  (-10.0f);
    camera->SetBottom (-10.0f);
    camera->SetTop    (10.0f);
    camera->SetZNear  (0.0f);
    camera->SetZFar   (1000.0f);    

    camera->SetPosition (0, 0.0f, -1.0f);

    scene_graph::Scene scene;
    
    camera->BindToScene (scene);

    scene_graph::Sprite::Pointer sprite = scene_graph::Sprite::Create ();

    sprite->SetMaterial ("sprite_material");
    sprite->SetPosition (math::vec3f (0.0f, 0.f, 1.f));
    sprite->SetScale (math::vec3f (10.0f));
    sprite->SetWorldOrientation (math::degree (45.0f), 0.0f, 0.0f, 1.0f);

    sprite->BindToScene (scene);

    scene_graph::Sprite::Pointer sprite2 = scene_graph::Sprite::Create ();

    sprite2->SetMaterial ("sprite_material");
    sprite2->SetPosition (math::vec3f (0.0f, 0.f, 2.f));
    sprite2->SetScale (math::vec3f (5.0f));
    sprite2->SetWorldOrientation (math::degree (45.0f), 0.0f, 0.0f, 1.0f);

    sprite2->BindToScene (scene, scene_graph::NodeBindMode_AddRef);

      //настройка области вывода

    scene_graph::Viewport vp;
    
    vp.SetArea       (0, 0, 100, 100);
    vp.SetCamera     (&*camera);
    vp.SetTechnique  ("default");
    
    screen.Attach (vp);

    window.Show ();    

    Test test (target, sprite);

    sprite = scene_graph::Sprite::Create ();

    syslib::Application::RegisterEventHandler (syslib::ApplicationEvent_OnIdle, xtl::bind (&idle, xtl::ref (test)));
    
    syslib::Application::Run ();
  }
  catch (std::exception& e)
  {
    printf ("%s\n", e.what ());
  }
}
