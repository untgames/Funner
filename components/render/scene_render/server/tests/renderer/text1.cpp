#include "shared.h"

struct Test
{
  RenderTarget&          target;
  scene_graph::TextLine& text_line;

  Test (RenderTarget& in_target, scene_graph::TextLine& in_text_line)
    : target (in_target)
    , text_line (in_text_line)
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
    
//    float angle = common::milliseconds () / 100.0f;
    
//    test.text_line.SetWorldOrientation (math::degree (angle), 0.0f, 0.0f, 1.0f);        

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

    media::FontLibrary font_library;

    font_library.LoadFont ("data/fonts/times.xfont");

    const char* SERVER_NAME = "MyServer";

    Server server (SERVER_NAME/*, render::scene::server::ServerThreadingModel_SingleThreaded*/);
    Client client (SERVER_NAME);

    common::PropertyMap window_properties;
    
    window_properties.SetProperty ("ColorBits", 32);
    window_properties.SetProperty ("DepthBits", 24);

    server.AttachWindow ("my_window", window, window_properties);

    client.AttachFontLibrary (font_library);

    client.LoadResource ("data/render.rfx");
    client.LoadResource ("data/text.xmtl");

//TODO: prefetch

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

//    camera->SetPosition (0, 0.0f, 1.0f);
//    camera->LookTo (math::vec3f (0.0f), math::vec3f (0, 1.0f, 0), scene_graph::NodeTransformSpace_World);

    scene_graph::Scene scene;
    
    camera->BindToScene (scene);

    scene_graph::DirectLight::Pointer light = scene_graph::DirectLight::Create ();

    light->BindToParent (*camera);

    scene_graph::TextLine::Pointer text_line (scene_graph::TextLine::Create (font_library));

    media::FontCreationParams creation_params = text_line->FontCreationParams ();

    creation_params.font_size = 50;

    text_line->SetTextUtf8 ("Hello world!");
    text_line->SetFont  ("Times New Roman");
    text_line->SetFontCreationParams (creation_params);
    text_line->SetScale (math::vec3f (1.2f));
    text_line->SetHorizontalAlignment (scene_graph::TextLineAlignment_Center);
    text_line->SetPosition (math::vec3f (0, 0, 1.0f));
    
    text_line->SetMaterial ("text_material");

    text_line->BindToScene (scene);
    
      //настройка области вывода
    
    scene_graph::Viewport vp;
    
    vp.SetArea       (0, 0, 100, 100);
    vp.SetCamera     (camera.get ());
    vp.SetTechnique  ("default");
    
    screen.Attach (vp);

    window.Show ();    

    Test test (target, *text_line);

    syslib::Application::RegisterEventHandler (syslib::ApplicationEvent_OnIdle, xtl::bind (&idle, xtl::ref (test)));
    
    syslib::Application::Run ();
  }
  catch (std::exception& e)
  {
    printf ("%s\n", e.what ());
  }
}
