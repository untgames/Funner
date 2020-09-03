#include "shared.h"

struct Test
{
  RenderTarget&                            target;
  scene_graph::HeightMap&                  height_map;
  scene_graph::controllers::Water::Pointer water;

  Test (RenderTarget& in_target, scene_graph::HeightMap& in_height_map, scene_graph::controllers::Water::Pointer in_water)
    : target (in_target)
    , height_map (in_height_map)
    , water (in_water)
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
      printf ("FPS: %.2f\n", float (frames_count)/float (common::milliseconds () - last_fps)*1000.f);
      fflush (stdout);

      float storm_x = rand () / (float)RAND_MAX * 0.8f - 0.4f,
            storm_y = rand () / (float)RAND_MAX * 0.8f - 0.4f;

      test.water->PutStorm (math::vec3f (storm_x, storm_y, 0), -0.15f, 0.005f);

/*      if (!test.height_map.Scissor())
      {
        printf ("SET SCISSOR\n");

        scene_graph::Scissor::Pointer scissor = scene_graph::Scissor::Create ();

        scissor->SetScale (math::vec3f (1.f));
        scissor->SetWorldPosition (math::vec3f (0.f, 2.f, 0.f));

        scissor->BindToScene (*test.height_map.Scene (), scene_graph::NodeBindMode_AddRef);

        test.height_map.SetScissor (scissor.get ());
      }*/

      last_fps = common::milliseconds ();
      frames_count = 0;
      return;
    }
    
    frames_count++;
    
//    float angle = common::milliseconds () / 100.0f;
    
//    test.height_map.SetWorldOrientation (math::degree (angle), 0.f, 0.f, 1.0f);

    test.height_map.Update (scene_graph::TimeValue (common::milliseconds (), 1000));
    test.target.Update ();      
  }
  catch (std::exception& e)
  {
    printf ("exception in idle: %s\n", e.what ());
  }
}

int main ()
{
  printf ("Results of water1_test:\n");

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
    client.LoadResource ("data/water.xmtl");

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

    camera->SetPosition (0, 0.0f, -20.0f);

    scene_graph::DirectLight::Pointer light = scene_graph::DirectLight::Create ();

    light->BindToParent (*camera);
//    camera->LookTo (math::vec3f (0.0f), math::vec3f (0, 1.0f, 0), scene_graph::NodeTransformSpace_World);

    scene_graph::Scene scene;
    
    camera->BindToScene (scene);
    
    scene_graph::HeightMap::Pointer height_map = scene_graph::HeightMap::Create ();
    
    height_map->SetCellsCount (256, 256);
    height_map->SetVerticesColor (math::vec4f (1.f));
    height_map->SetVerticesNormal (math::vec3f (0.f, 0.f, -1.f));
    height_map->SetMaterial ("water");
    height_map->SetScale (math::vec3f (20.0f, 20.f, 1.f));
    height_map->SetWorldOrientation (math::degree (45.0f), 1.0f, 0.0f, 0.0f);

    height_map->BindToScene (scene);
    
    scene_graph::controllers::Water::Pointer water = scene_graph::controllers::Water::Create (*height_map);

      //настройка области вывода
    
    scene_graph::Viewport vp;
    
    vp.SetArea       (0, 0, 100, 100);
    vp.SetCamera     (camera.get ());
    vp.SetTechnique  ("default");
    
    screen.Attach (vp);

    window.Show ();    

    Test test (target, *height_map, water);

    syslib::Application::RegisterEventHandler (syslib::ApplicationEvent_OnIdle, xtl::bind (&idle, xtl::ref (test)));
    
    syslib::Application::Run ();
  }
  catch (std::exception& e)
  {
    printf ("%s\n", e.what ());
  }
}
