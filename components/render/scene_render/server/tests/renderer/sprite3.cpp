#include "shared.h"

struct Test
{
  RenderTarget&                             target;
  scene_graph::Scene                        scene;
  stl::vector<scene_graph::Sprite::Pointer> sprites;

  Test (RenderTarget& in_target)
    : target (in_target)
  {
  }

  void CreateSprites ()
  {
    for (size_t i = 0, count = sprites.size (); i < count; i++)
      sprites [i]->Unbind ();

    sprites.clear ();

    for (size_t i = 0; i < 10; i++)
    {
      for (size_t j = 0; j < 10; j++)
      {
        scene_graph::Sprite::Pointer sprite = scene_graph::Sprite::Create ();

        sprite->SetMaterial ("sprite_material");

        sprite->SetPosition (math::vec3f (-9.f + j * 1.8f, -9.f + i * 1.8f, 1.f));

        sprite->SetScale (math::vec3f (1.f));

        sprite->BindToScene (scene);

        sprites.push_back (sprite);
      }
    }
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

    if (common::milliseconds () - last_fps > 1)
    {
      test.CreateSprites ();

      printf ("FPS: %.2f\n", float (frames_count)/float (common::milliseconds () - last_fps)*1000.f);
      fflush (stdout);

      last_fps = common::milliseconds ();
      frames_count = 0;
      return;
    }
    
    frames_count++;

    test.target.Update ();      
  }
  catch (std::exception& e)
  {
    printf ("exception in idle: %s\n", e.what ());
  }  
}

int main ()
{
  printf ("Results of sprite3_test:\n");

  try
  {
    common::LogFilter log_filter ("render.*", &log_print);

    syslib::Window window (syslib::WindowStyle_Overlapped, 400, 400);

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
    
    camera->SetLeft   (-10.0f);
    camera->SetRight  (10.0f);
    camera->SetBottom (-10.0f);
    camera->SetTop    (10.0f);
    camera->SetZNear  (0.0f);
    camera->SetZFar   (1000.0f);    

    camera->SetPosition (0, 0.0f, -1.0f);

    Test test (target);

    camera->BindToScene (test.scene);

      //настройка области вывода

    scene_graph::Viewport vp;
    
    vp.SetArea       (0, 0, 100, 100);
    vp.SetCamera     (camera.get ());
    vp.SetTechnique  ("default");
    
    screen.Attach (vp);

    window.Show ();    

    syslib::Application::RegisterEventHandler (syslib::ApplicationEvent_OnIdle, xtl::bind (&idle, xtl::ref (test)));
    
    syslib::Application::Run ();
  }
  catch (std::exception& e)
  {
    printf ("%s\n", e.what ());
  }
}
