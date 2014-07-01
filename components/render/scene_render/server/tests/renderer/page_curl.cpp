#include "shared.h"

struct Test
{
  RenderTarget&          target;
  scene_graph::PageCurl& model;

  Test (RenderTarget& in_target, scene_graph::PageCurl& in_model)
    : target (in_target)
    , model (in_model)
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

      last_fps = common::milliseconds ();
      frames_count = 0;
      return;
    }
    
    frames_count++;
    
    float angle = common::milliseconds () / 100.0f;
    
    test.model.SetWorldOrientation (math::degree (angle), 0.0f, 0.0f, 1.0f);

    test.target.Update ();      
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

    syslib::Window window (syslib::WindowStyle_Overlapped, 400, 300);

    window.RegisterEventHandler (syslib::WindowEvent_OnClose, xtl::bind (&on_window_close));

    const char* SERVER_NAME = "MyServer";

    Server server (SERVER_NAME);
    Client client (SERVER_NAME);

    common::PropertyMap window_properties;
    
    window_properties.SetProperty ("ColorBits", 32);
    window_properties.SetProperty ("DepthBits", 24);

    server.AttachWindow ("my_window", window, window_properties);

    client.LoadResource ("data/render.rfx");
    client.LoadResource ("data/u1/texture0000.dds");
    client.LoadResource ("data/u1/texture0001.dds");
    client.LoadResource ("data/u1/texture0002.dds");
    client.LoadResource ("data/u1/EnvGala_020_D.dds");
    client.LoadResource ("data/page_curl.xmtl");

    RenderTarget target = client.CreateRenderTarget ("my_window");

    scene_graph::Screen screen;
    
    target.SetScreen (&screen);
    
    screen.SetBackgroundState (true);
    screen.SetBackgroundColor (math::vec4f (0.0f, 1.0f, 1.0f, 1.0f));

    scene_graph::OrthoCamera::Pointer camera = scene_graph::OrthoCamera::Create ();
    
    camera->SetLeft   (-1.1f);
    camera->SetRight  (1.1f);
    camera->SetBottom (-0.9f);
    camera->SetTop    (0.9f);
    camera->SetZNear  (-2.0f);
    camera->SetZFar   (2.0f);

    scene_graph::Scene scene;
    
    camera->BindToScene (scene);
    
    scene_graph::PageCurl::Pointer model = scene_graph::PageCurl::Create ();
    
    model->SetSize               (2.f, 1.5f);
    model->SetGridSize           (100, 100);
    model->SetMode               (scene_graph::PageCurlMode_DoublePageDoubleMaterial);
    model->SetCurlPoint          (scene_graph::PageCurlCorner_RightBottom);
    model->SetCurlRadius         (0.1f);
    model->SetPageMaterial       (scene_graph::PageCurlPageType_BackLeft,   "page1");
    model->SetPageMaterial       (scene_graph::PageCurlPageType_BackRight,  "page2");
    model->SetPageMaterial       (scene_graph::PageCurlPageType_FrontLeft,  "page3");
    model->SetPageMaterial       (scene_graph::PageCurlPageType_FrontRight, "page4");
    model->SetCornerShadowOffset (0.1f);
    model->SetCurlPointPosition  (0.1f, 0.1f);
    model->SetRigidPage          (true);
    model->SetRigidPagePerspectiveFactor (1.2f);

    model->BindToScene (scene);    
    
      //настройка области вывода
    
    scene_graph::Viewport vp;
    
    vp.SetArea       (0, 0, 100, 100);
    vp.SetCamera     (&*camera);
    vp.SetTechnique  ("default");
    
    screen.Attach (vp);

    window.Show ();    

    Test test (target, *model);

    syslib::Application::RegisterEventHandler (syslib::ApplicationEvent_OnIdle, xtl::bind (&idle, xtl::ref (test)));
    
    syslib::Application::Run ();
  }
  catch (std::exception& e)
  {
    printf ("%s\n", e.what ());
  }
}
