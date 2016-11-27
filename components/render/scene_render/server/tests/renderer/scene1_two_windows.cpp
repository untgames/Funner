#include "shared.h"

struct Test
{
  RenderTarget&            target;
  RenderTarget&            target2;
  scene_graph::StaticMesh& mesh;

  Test (RenderTarget& in_target, RenderTarget& in_target2, scene_graph::StaticMesh& in_mesh)
    : target (in_target)
    , target2 (in_target2)
    , mesh (in_mesh)
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

void idle (Test& test, scene_graph::Light& light)
{
  try
  {
    static size_t last_fps = 0;
    static size_t frames_count = 0;

    if (common::milliseconds () - last_fps > 1000)
    {
      printf ("FPS: %.2f\n", float (frames_count)/float (common::milliseconds () - last_fps)*1000.f);
      fflush (stdout);

      last_fps = common::milliseconds ();
      frames_count = 0;
      return;
    }
    
    frames_count++;
    
    float angle = common::milliseconds () / 100.0f;
    
    test.mesh.SetWorldOrientation (math::degree (angle), 0.0f, 0.0f, 1.0f);        

//    static float LIGHT_R = 100.0f;

//    float light_x = cos (math::radian (math::degree (-angle))) * LIGHT_R,
//          light_z = sin (math::radian (math::degree (-angle))) * LIGHT_R;

//    light.SetPosition  (light_x, 400.0f, light_z);
//    light.SetPosition  (10, 0.0f, 0);
//    light.LookTo       (math::vec3f (0.0f), scene_graph::NodeOrt_Z, scene_graph::NodeOrt_Y, scene_graph::NodeTransformSpace_World);

    test.target.Update ();
    test.target2.Update ();
  }
  catch (std::exception& e)
  {
    printf ("exception in idle: %s\n", e.what ());
  }  
}

int main ()
{
  printf ("Results of scene1_test:\n");

  try
  {
    common::LogFilter log_filter ("render.*", &log_print);

    syslib::Window window (syslib::WindowStyle_Overlapped, 512, 512);
    syslib::Window window2 (syslib::WindowStyle_Overlapped, 512, 512);

    window.RegisterEventHandler (syslib::WindowEvent_OnClose, xtl::bind (&on_window_close));

    const char* SERVER_NAME = "MyServer";

    Server server (SERVER_NAME, render::scene::server::ServerThreadingModel_SingleThreaded);
    Client client (SERVER_NAME);

    common::PropertyMap window_properties;
    
    window_properties.SetProperty ("ColorBits", 32);
    window_properties.SetProperty ("DepthBits", 24);

    server.AttachWindow ("my_window", window, window_properties);
    server.AttachWindow ("my_window2", window2, window_properties);

    client.LoadResource ("data/render.rfx");
    client.LoadResource ("data/u1/texture0000.dds");
    client.LoadResource ("data/u1/texture0001.dds");
    client.LoadResource ("data/u1/texture0002.dds");
    client.LoadResource ("data/u1/EnvGala_020_D.dds");
    client.LoadResource ("data/u1.xmtl");
    client.LoadResource ("data/u1.xmesh");
    client.LoadResource ("data/quad.xmesh");

    RenderTarget target = client.CreateRenderTarget ("my_window");
    RenderTarget target2 = client.CreateRenderTarget ("my_window2");

    scene_graph::Screen screen;
    scene_graph::Screen screen2;
    
    target.SetScreen (&screen);
    target2.SetScreen (&screen2);
    
//    screen.SetBackgroundState (true);
    screen.SetBackgroundColor (math::vec4f (0.0f, 1.0f, 1.0f, 1.0f));

  //  screen2.SetBackgroundState (true);
    screen2.SetBackgroundColor (math::vec4f (1.0f, 0.0f, 1.0f, 1.0f));

    scene_graph::OrthoCamera::Pointer camera = scene_graph::OrthoCamera::Create ();
    
    camera->SetLeft   (-10.0f);
    camera->SetRight  (10.0f);
    camera->SetBottom (-10.0f);
    camera->SetTop    (10.0f);
    camera->SetZNear  (-1000.0f);
    camera->SetZFar   (1000.0f);    

    camera->SetPosition (0, 10.0f, 0.0f);
    camera->LookTo      (math::vec3f (0.0f), scene_graph::NodeOrt_Z, scene_graph::NodeOrt_X, scene_graph::NodeTransformSpace_World);
//    camera->LookTo      (math::vec3f (0.0f), math::vec3f (0, 1.0f, 0), scene_graph::NodeTransformSpace_World);
    
    scene_graph::Scene scene;
    
    camera->BindToScene (scene);


    scene_graph::OrthoCamera::Pointer camera2 = scene_graph::OrthoCamera::Create ();

    camera2->SetLeft   (-10.0f);
    camera2->SetRight  (10.0f);
    camera2->SetBottom (-10.0f);
    camera2->SetTop    (10.0f);
    camera2->SetZNear  (-1000.0f);
    camera2->SetZFar   (1000.0f);

    camera2->SetPosition (0, 10.0f, 0.0f);
    camera2->LookTo      (math::vec3f (0.0f), scene_graph::NodeOrt_Z, scene_graph::NodeOrt_X, scene_graph::NodeTransformSpace_World);

    camera2->BindToScene (scene);





    scene_graph::OrthoCamera::Pointer camera3 = scene_graph::OrthoCamera::Create ();

    camera3->SetLeft   (-10.0f);
    camera3->SetRight  (10.0f);
    camera3->SetBottom (-10.0f);
    camera3->SetTop    (10.0f);
    camera3->SetZNear  (-1000.0f);
    camera3->SetZFar   (1000.0f);

    camera3->SetPosition (0, 10.0f, 0.0f);
    camera3->LookTo      (math::vec3f (0.0f), scene_graph::NodeOrt_Z, scene_graph::NodeOrt_X, scene_graph::NodeTransformSpace_World);

    camera3->BindToScene (scene);





    scene_graph::OrthoCamera::Pointer camera4 = scene_graph::OrthoCamera::Create ();

    camera4->SetLeft   (-10.0f);
    camera4->SetRight  (10.0f);
    camera4->SetBottom (-10.0f);
    camera4->SetTop    (10.0f);
    camera4->SetZNear  (-1000.0f);
    camera4->SetZFar   (1000.0f);

    camera4->SetPosition (0, 10.0f, 0.0f);
    camera4->LookTo      (math::vec3f (0.0f), scene_graph::NodeOrt_Z, scene_graph::NodeOrt_X, scene_graph::NodeTransformSpace_World);

    camera4->BindToScene (scene);


    
    scene_graph::StaticMesh::Pointer model = scene_graph::StaticMesh::Create ();
    
    model->SetMeshName ("u1.polySurface2.mesh#0");

    model->BindToScene (scene);

    scene_graph::StaticMesh::Pointer model2 = scene_graph::StaticMesh::Create ();

    model2->SetMeshName ("quad");
    model2->Rotate (math::degree (-90.f), math::degree (0.f), math::degree (0.f));
    model2->SetPosition (0, -8.0f, 0);
    model2->Scale (10.0f, 10.0f, 10.0f);

    model2->BindToScene (scene);

    scene_graph::SpotLight::Pointer light = scene_graph::SpotLight::Create ();

    light->SetPosition  (0.0f, 10.0f, 0);
    light->SetRange     (20.0f);
    light->LookTo       (math::vec3f (0.0f), scene_graph::NodeOrt_Z, scene_graph::NodeOrt_X, scene_graph::NodeTransformSpace_World);
    light->SetIntensity (1.0f);
    light->SetAngle     (math::degree (100.0f));

    light->BindToScene (scene);
    
      //пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅ
    
    scene_graph::Viewport vp;
    
    vp.SetArea       (0, 0, 100, 100);
    vp.SetCamera     (camera.get ());
    vp.SetTechnique  ("default");
    vp.SetMinDepth   (0);
    vp.SetMaxDepth   (0.2);
    
    screen.Attach (vp);

    scene_graph::Viewport vp2;

    vp2.SetArea       (0, 0, 100, 100);
    vp2.SetCamera     (camera2.get ());
    vp2.SetTechnique  ("default");
    vp2.SetMinDepth   (0.3);
    vp2.SetMaxDepth   (0.5);

    screen2.Attach (vp2);

    window.Show ();    
    window2.Show ();

    window.SetPosition (100, 100);
    window2.SetPosition (700, 100);

    Test test (target, target2, *model);

    syslib::Application::RegisterEventHandler (syslib::ApplicationEvent_OnIdle, xtl::bind (&idle, xtl::ref (test), xtl::ref (*light)));
    
    syslib::Application::Run ();
  }
  catch (std::exception& e)
  {
    printf ("%s\n", e.what ());
  }
}
