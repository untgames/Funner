#include "shared.h"

//получение ортографической матрицы проекции
math::mat4f get_ortho_proj (float left, float right, float bottom, float top, float znear, float zfar)
{
  math::mat4f proj_matrix;

  float width  = right - left,
        height = top - bottom,
        depth  = zfar - znear;

    //выбрана матрица проецирования, используемая gluOrtho2D

  proj_matrix [0] = math::vec4f (2.0f / width, 0, 0, - (right + left) / width);
  proj_matrix [1] = math::vec4f (0, 2.0f / height, 0, - (top + bottom) / height);
  proj_matrix [2] = math::vec4f (0, 0, -2.0f / depth, - (znear + zfar) / depth);
  proj_matrix [3] = math::vec4f (0, 0, 0, 1);

  return proj_matrix;
}

void idle (Test& test, Entity& entity, Frame& frame)
{
  try
  {
    static size_t last = 0;
    static float angle;

    static size_t last_fps = 0;
    static size_t frames_count = 0;

    float dt = float (common::milliseconds () - last) / 1000.f;

    if (common::milliseconds () - last > 25)
    {
      last = common::milliseconds ();
      return;
    }

    if (common::milliseconds () - last_fps > 1000)
    {
      printf ("FPS: %.2f\n", float (frames_count)/float (common::milliseconds () - last_fps)*1000.f);
      fflush (stdout);

      last_fps = common::milliseconds ();
      frames_count = 0;        
      
      return;
    }
    
    static size_t last_settings_change = 0;
    
    if (common::milliseconds () - last_settings_change > 5000)
    {
      static bool state = false;
      
      common::PropertyMap settings;
      
      settings.SetProperty ("FrontCounterClockwise", state ? "true" : "false");
      
      test.RenderManager ().ChangeSettings (settings);    
      
      state = !state;
      
      last_settings_change = common::milliseconds ();
    }    
    
    frames_count++;
    
    common::PropertyMap frame_properties = frame.Properties ();
    common::PropertyMap entity_properties = entity.Properties ();  

    angle += 0.05f*dt;
    
    entity_properties.SetProperty ("myObjectMatrix", math::rotate (math::radian (angle), math::vec3f (0, 0, 1)) *
      math::rotate (math::radian (angle*0.2f), math::vec3f (1, 0, 0)));
      
    math::vec3f light_pos = math::vec3f (40 * cos (angle), 40 * sin (angle), 0.0f);
      
    frame_properties.SetProperty ("lightPos", light_pos);
    frame_properties.SetProperty ("lightDir", -math::normalize (light_pos));

    frame.Draw ();
      
    test.Window ().SwapBuffers ();
  }
  catch (std::exception& e)
  {
    printf ("exception: %s\n    at idle", e.what ());
  }
  catch (...)
  {
    printf ("unknown exception\n    at idle");
  }
}

int main ()
{
  printf ("Results of model_load_ffp_test:\n");
  
  try
  {
    Test test (L"Model load FFP", true);

    RenderManager render_manager = test.RenderManager ();
    
    common::PropertyMap settings;
    
    settings.SetProperty ("FrontCounterClockwise", "false");
    
    render_manager.ChangeSettings (settings);

    render_manager.LoadResource ("data/spy.xmesh");
    render_manager.LoadResource ("data/spy.xmtl");
    render_manager.LoadResource ("data/spy.rfx");    
    
    Entity entity = render_manager.CreateEntity ();
    
    entity.SetPrimitive ("spy_body");
    
    Frame frame = render_manager.CreateFrame ();

    frame.DisableAutoCleanup ();            
    frame.SetRenderTarget ("main_color_target", test.Window ().ColorBuffer ());
    frame.SetRenderTarget ("main_depth_stencil_target", test.Window ().DepthStencilBuffer ());
    frame.SetEffect ("main");
    frame.SetClearColor (math::vec4f (0.0f, 0.0f, 1.0f, 1.0f));
    
    common::PropertyMap frame_properties = frame.Properties ();
    common::PropertyMap entity_properties = entity.Properties ();
    
    frame_properties.SetProperty ("myProjMatrix", get_ortho_proj (-100, 100, -100, 100, -1000, 1000));
    frame_properties.SetProperty ("myViewMatrix", inverse (math::lookat (math::vec3f (0, 400, 0), math::vec3f (0.0f), math::vec3f (0, 0, 1))));
    entity_properties.SetProperty ("myObjectMatrix", math::mat4f (1.0f));        
    
    frame.AddEntity (entity); 
    
    test.ShowWindow ();
    
    syslib::Application::RegisterEventHandler (syslib::ApplicationEvent_OnIdle, xtl::bind (&idle, xtl::ref (test), xtl::ref (entity), xtl::ref (frame)));
            
    return test.Run ();
  }
  catch (std::exception& e)
  {
    printf ("%s\n", e.what ());
  }

  return 0;
}
