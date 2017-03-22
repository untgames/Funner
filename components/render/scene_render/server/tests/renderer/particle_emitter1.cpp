#include "shared.h"

struct Test
{
  RenderTarget&                 target;
  scene_graph::ParticleEmitter& emitter;

  Test (RenderTarget& in_target, scene_graph::ParticleEmitter& in_emitter)
    : target (in_target)
    , emitter (in_emitter)
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
    static int    spawn_enabled = 0;
    static int    update_enabled = 1;

    size_t current_time = common::milliseconds ();

    if (current_time - last_fps > 1000)
    {
      printf ("FPS: %.2f\n", float (frames_count)/float (current_time - last_fps)*1000.f);
      fflush (stdout);

/*      if (!test.emitter.Scissor())
      {
        printf ("SET SCISSOR\n");

        scene_graph::Scissor::Pointer scissor = scene_graph::Scissor::Create ();

        scissor->SetScale (math::vec3f (50.f));
        scissor->SetWorldPosition (math::vec3f (0.f, 2.f, 0.f));

        scissor->BindToScene (*test.emitter.Scene (), scene_graph::NodeBindMode_AddRef);

        test.emitter.SetScissor (scissor.get ());
      }*/

      last_fps = current_time;
      frames_count = 0;
      spawn_enabled++;
      update_enabled++;

      common::PropertyMap node_properties;

      node_properties.SetProperty ("particles.ParticleDesigner.EmitCountMultiplier", spawn_enabled % 4 ? 0.f : 1.f);

      test.emitter.SetProperties (&node_properties);

      return;
    }

    test.emitter.Scale (1.f, 1.f , 1.f);
    
    frames_count++;
    
//    float angle = common::milliseconds () / 100.0f;
    
//    test.emitter.SetWorldOrientation (math::degree (angle), 0.0f, 0.0f, 1.0f);

    if (update_enabled % 4)
      test.emitter.Update (scene_graph::TimeValue (current_time, 1000));

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
//    common::LogFilter log_filter ("render.*", &log_print);

    syslib::Window window (syslib::WindowStyle_Overlapped, 400, 400);

    window.RegisterEventHandler (syslib::WindowEvent_OnClose, xtl::bind (&on_window_close));

    media::particles::ParticleSystemLibrary particle_system_library;

    particle_system_library.Load ("data/smoke.plist");

   const char* SERVER_NAME = "MyServer";

    Server server (SERVER_NAME/*, render::scene::server::ServerThreadingModel_SingleThreaded*/);
    Client client (SERVER_NAME);

    client.AttachParticleSystemLibrary (particle_system_library);

    common::PropertyMap window_properties;
    
    window_properties.SetProperty ("ColorBits", 32);
    window_properties.SetProperty ("DepthBits", 24);

    server.AttachWindow ("my_window", window, window_properties);

    client.LoadResource ("data/render.rfx");
    client.LoadResource ("data/smoke.png");
    client.LoadResource ("data/smoke.xmtl");

    RenderTarget target = client.CreateRenderTarget ("my_window");

    scene_graph::Screen screen;
    
    target.SetScreen (&screen);
    
    screen.SetBackgroundState (true);
    screen.SetBackgroundColor (math::vec4f (0.0f, 1.0f, 1.0f, 1.0f));

    scene_graph::OrthoCamera::Pointer camera = scene_graph::OrthoCamera::Create ();
    
    camera->SetLeft   (-100.0f);
    camera->SetRight  (100.0f);
    camera->SetBottom (-100.0f);
    camera->SetTop    (100.0f);
    camera->SetZNear  (0.0f);
    camera->SetZFar   (100.0f);

    camera->SetPosition (0, 0.0f, -1.0f);

    scene_graph::Scene scene;
    
    camera->BindToScene (scene);
    
    scene_graph::DirectLight::Pointer light = scene_graph::DirectLight::Create ();

    light->BindToParent (*camera);

    scene_graph::ParticleEmitter::Pointer emitter = scene_graph::ParticleEmitter::Create ("data/smoke.plist");
    
/*    emitter->SetPosition (0, 0, 1);
    emitter->SetScale (math::vec3f (0.8f));
    emitter->SetWorldOrientation (math::degree (45.0f), 0.0f, 0.0f, 1.0f);*/

    emitter->BindToScene (scene);
    
      //настройка области вывода
    
    scene_graph::Viewport vp;
    
    vp.SetArea       (0, 0, 100, 100);
    vp.SetCamera     (camera.get ());
    vp.SetTechnique  ("default");
    
    screen.Attach (vp);

    window.Show ();    

    Test test (target, *emitter);

    syslib::Application::RegisterEventHandler (syslib::ApplicationEvent_OnIdle, xtl::bind (&idle, xtl::ref (test)));
    
    syslib::Application::Run ();
  }
  catch (std::exception& e)
  {
    printf ("%s\n", e.what ());
  }
}
