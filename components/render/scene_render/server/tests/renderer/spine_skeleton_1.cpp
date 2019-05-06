#include "shared.h"

//const char* SPINE_FILE_NAME  = "data/alien/export/alien-ess.skel";
//const char* SPINE_ATLAS_NAME = "data/alien/export/alien.atlas";
const char* SPINE_FILE_NAME  = "data/alien/export/alien-pro.skel";
const char* SPINE_ATLAS_NAME = "data/alien/export/alien.atlas";
//const char* SPINE_FILE_NAME  = "data/coin/export/coin-pro.skel";
//const char* SPINE_ATLAS_NAME = "data/coin/export/coin.atlas";
//const char* SPINE_FILE_NAME  = "data/dragon/export/dragon-ess.skel";
//const char* SPINE_ATLAS_NAME = "data/dragon/export/dragon.atlas";
//const char* SPINE_FILE_NAME  = "data/goblins/export/goblins-ess.skel";
//const char* SPINE_ATLAS_NAME = "data/goblins/export/goblins.atlas";
//const char* SPINE_FILE_NAME  = "data/goblins/export/goblins-pro.skel";
//const char* SPINE_ATLAS_NAME = "data/goblins/export/goblins.atlas";
//const char* SPINE_FILE_NAME  = "data/hero/export/hero-ess.skel";
//const char* SPINE_ATLAS_NAME = "data/hero/export/hero.atlas";
//const char* SPINE_FILE_NAME  = "data/hero/export/hero-pro.skel";
//const char* SPINE_ATLAS_NAME = "data/hero/export/hero.atlas";
//const char* SPINE_FILE_NAME  = "data/powerup/export/powerup-ess.skel";
//const char* SPINE_ATLAS_NAME = "data/powerup/export/powerup.atlas";
//const char* SPINE_FILE_NAME  = "data/powerup/export/powerup-pro.skel";
//const char* SPINE_ATLAS_NAME = "data/powerup/export/powerup.atlas";
//const char* SPINE_FILE_NAME  = "data/raptor/export/raptor-pro.skel";
//const char* SPINE_ATLAS_NAME = "data/raptor/export/raptor.atlas";
//const char* SPINE_FILE_NAME  = "data/speedy/export/speedy-ess.skel";
//const char* SPINE_ATLAS_NAME = "data/speedy/export/speedy.atlas";
//const char* SPINE_FILE_NAME  = "data/spineboy/export/spineboy-ess.skel";
//const char* SPINE_ATLAS_NAME = "data/spineboy/export/spineboy.atlas";
//const char* SPINE_FILE_NAME  = "data/spineboy/export/spineboy-pro.skel";
//const char* SPINE_ATLAS_NAME = "data/spineboy/export/spineboy.atlas";
//const char* SPINE_FILE_NAME  = "data/stretchyman/export/stretchyman-pro.skel";
//const char* SPINE_ATLAS_NAME = "data/stretchyman/export/stretchyman.atlas";
//const char* SPINE_FILE_NAME  = "data/tank/export/tank-pro.skel";
//const char* SPINE_ATLAS_NAME = "data/tank/export/tank.atlas";
//const char* SPINE_FILE_NAME  = "data/vine/export/vine-pro.json";
//const char* SPINE_ATLAS_NAME = "data/vine/export/vine.atlas";

struct Test
{
  RenderTarget&                       target;
  scene_graph::SpineSkeleton::Pointer skeleton;

  Test (RenderTarget& in_target, scene_graph::SpineSkeleton::Pointer in_skeleton)
    : target (in_target)
    , skeleton (in_skeleton)
  {
  }
};

unsigned int manual_time = 0;

void log_print (const char* log_name, const char* message)
{
  printf ("%s: %s\n", log_name, message);
  fflush (stdout);
}

void on_window_close ()
{
  syslib::Application::Exit (0);
}

void on_window_key_down ()
{
  manual_time += 100;
}

void idle (Test& test)
{
  try
  {
    static size_t last_fps = 0;
    static size_t frames_count = 0;

    if (common::milliseconds () - last_fps > 1000)
    {
      if (test.skeleton)
      {
//        test.sprite->Unbind ();
//        test.sprite = 0;
      }

      printf ("FPS: %.2f\n", float (frames_count)/float (common::milliseconds () - last_fps)*1000.f);
      fflush (stdout);

      last_fps = common::milliseconds ();
      frames_count = 0;
      return;
    }
    
    frames_count++;
    
    float angle = common::milliseconds () / 100.0f;

    if (test.skeleton)
      test.skeleton->SetWorldOrientation (math::degree (0.f), 0.0f, 0.0f, 1.0f);   //TODO just to update on each frame
//      test.skeleton->SetWorldOrientation (math::degree (angle), 0.0f, 0.0f, 1.0f);

//    test.skeleton->Update (scene_graph::TimeValue (manual_time, 1000));
    test.skeleton->Update (scene_graph::TimeValue (common::milliseconds (), 1000));

    test.target.Update ();
  }
  catch (std::exception& e)
  {
    printf ("exception in idle: %s\n", e.what ());
  }  
}

int main ()
{
  printf ("Results of spine_skeleton1_test:\n");

  try
  {
//    common::LogFilter log_filter ("render.*", &log_print);

    syslib::Window window (syslib::WindowStyle_Overlapped, 400, 300);

    window.RegisterEventHandler (syslib::WindowEvent_OnKeyDown, xtl::bind (&on_window_key_down));
    window.RegisterEventHandler (syslib::WindowEvent_OnClose, xtl::bind (&on_window_close));

    const char* SERVER_NAME = "MyServer";

    Server server (SERVER_NAME, render::scene::server::ServerThreadingModel_MultiThreaded);
    Client client (SERVER_NAME);

    common::PropertyMap window_properties;
    
    window_properties.SetProperty ("ColorBits", 32);
    window_properties.SetProperty ("DepthBits", 24);
//    window_properties.SetProperty ("VSync", 1);

    server.AttachWindow ("my_window", window, window_properties);

    client.LoadResource ("data/render_spine.rfx");
    client.LoadResource ("data/spine.xmtl");
    client.LoadResource ("data/alien/export/alien.png");
    client.LoadResource ("data/coin/export/coin.png");
    client.LoadResource ("data/dragon/export/dragon.png");
    client.LoadResource ("data/dragon/export/dragon2.png");
    client.LoadResource ("data/goblins/export/goblins.png");
    client.LoadResource ("data/hero/export/hero.png");
    client.LoadResource ("data/powerup/export/powerup.png");
    client.LoadResource ("data/raptor/export/raptor.png");
    client.LoadResource ("data/speedy/export/speedy.png");
    client.LoadResource ("data/spineboy/export/spineboy.png");
    client.LoadResource ("data/stretchyman/export/stretchyman.png");
    client.LoadResource ("data/tank/export/tank.png");
    client.LoadResource ("data/vine/export/vine.png");

    RenderTarget target = client.CreateRenderTarget ("my_window");

    scene_graph::Screen screen;
    
    target.SetScreen (&screen);
    
    screen.SetBackgroundState (true);
    screen.SetBackgroundColor (math::vec4f (0.4f, 0.4f, 0.4f, 1.0f));

    scene_graph::OrthoCamera::Pointer camera = scene_graph::OrthoCamera::Create ();
    
    camera->SetLeft   (-10.0f);
    camera->SetRight  (10.0f);
    camera->SetBottom (-10.0f);
    camera->SetTop    (10.0f);
    camera->SetZNear  (0.0f);
    camera->SetZFar   (1000.0f);    

    camera->SetPosition (0, 0.0f, -1.0f);

    scene_graph::Scene scene;
    
    camera->BindToScene (scene);

    scene_graph::DirectLight::Pointer light = scene_graph::DirectLight::Create ();

    light->BindToParent (*camera);

    media::spine::SkeletonData skeleton_data (SPINE_FILE_NAME, SPINE_ATLAS_NAME);
    media::spine::Skeleton media_skeleton = skeleton_data.CreateSkeleton ();

//    media_skeleton.SetSkin (skeleton_data.SkinName (2));   //for goblins

    scene_graph::SpineSkeleton::Pointer skeleton = scene_graph::SpineSkeleton::Create (media_skeleton);

    //add animation
    media::spine::AnimationStateData animation_state_data = skeleton_data.CreateAnimationStateData ();
    media::spine::AnimationState     animation_state      = animation_state_data.CreateAnimationState ();

    animation_state.EnqueueAnimation (0, skeleton_data.AnimationName (0), true, 0);

    scene_graph::controllers::SpineAnimation::Pointer                      animation                = scene_graph::controllers::SpineAnimation::Create (*skeleton, animation_state);
    scene_graph::controllers::SpineSkeletonVisualStructureBuilder::Pointer visual_structure_builder = scene_graph::controllers::SpineSkeletonVisualStructureBuilder::Create (*skeleton, 1.f, 0.f);

    skeleton->SetPosition (math::vec3f (0.0f, -5.f, 1.f));
    skeleton->SetScale (math::vec3f (0.002f));
    //skeleton->SetWorldOrientation (math::degree (45.0f), 0.0f, 0.0f, 1.0f);

    skeleton->BindToScene (scene);

      //viewport setup

    scene_graph::Viewport vp;
    
    vp.SetArea       (0, 0, 100, 100);
    vp.SetCamera     (camera.get ());
    vp.SetTechnique  ("default");
    
    screen.Attach (vp);

    window.Show ();    

    Test test (target, skeleton);

    syslib::Application::RegisterEventHandler (syslib::ApplicationEvent_OnIdle, xtl::bind (&idle, xtl::ref (test)));
    
    syslib::Application::Run ();
  }
  catch (std::exception& e)
  {
    printf ("%s\n", e.what ());
  }
}
