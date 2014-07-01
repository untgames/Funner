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

void update_entity_frame_transformations (Frame& frame, Entity& entity, void* user_data, EntityDrawParams& out_params)
{
  PropertyMap properties = out_params.properties;
  
  math::mat4f model_view_tm = frame.Properties ().GetMatrix ("ViewMatrix") * entity.Properties ().GetMatrix ("ObjectMatrix");  

  out_params.mvp_matrix = frame.Properties ().GetMatrix ("ProjectionMatrix") * model_view_tm;

  properties.SetProperty ("ModelViewMatrix", model_view_tm);
  properties.SetProperty ("ModelViewProjectionMatrix", out_params.mvp_matrix);
}

void idle (Test& test, Entity& entity, Frame& frame)
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
    
    common::PropertyMap frame_properties = frame.Properties ();
    common::PropertyMap entity_properties = entity.Properties ();  

    float angle = common::milliseconds () / 10000.0f;
    
    entity_properties.SetProperty ("ObjectMatrix", math::rotate (math::radian (angle), math::vec3f (0, 0, 1)) * math::rotate (math::radian (angle*0.2f), math::vec3f (1, 0, 0)));        
      
    math::vec4f light_dir [2] = {
      normalize (math::vec4f (cos (angle), sin (angle), 0.0f, 0.0f)),
      normalize (math::vec4f (sin (angle), 0.0f, cos (angle), 0.0f))
    };
    
    frame_properties.SetProperty ("DirectLightWorldDirection", 2, light_dir);    
        
    frame.Draw ();
      
    test.Window ().SwapBuffers ();
  }
  catch (xtl::exception& e)
  {
    printf ("exception in idle: %s\n", e.what ());
  }  
}

int main ()
{
  printf ("Results of model_load_ffp_test:\n");
  
  try
  {
    Test test (L"Model load FFP", true);

    RenderManager render_manager = test.RenderManager ();

    render_manager.LoadResource ("data/u1.binmesh");
    render_manager.LoadResource ("data/u1.xmtl");
    render_manager.LoadResource ("data/u1.rfx");
    render_manager.LoadResource ("data/u1/texture0000.dds");
    render_manager.LoadResource ("data/u1/texture0001.dds");
    render_manager.LoadResource ("data/u1/texture0002.dds");
    
    Entity entity = render_manager.CreateEntity ();
    
    entity.SetPrimitive ("u1.polySurface2.mesh#0");
    
    Frame frame = render_manager.CreateFrame ();

    frame.DisableAutoCleanup ();           
    frame.SetRenderTarget ("main_color_target", test.Window ().ColorBuffer ());
    frame.SetRenderTarget ("main_depth_stencil_target", test.Window ().DepthStencilBuffer ());
    frame.SetEffect ("main");
    frame.SetClearColor (math::vec4f (0.0f, 0.0f, 1.0f, 1.0f));
    frame.SetEntityDrawHandler (&update_entity_frame_transformations);
    
    common::PropertyMap frame_properties = frame.Properties ();
    common::PropertyMap entity_properties = entity.Properties ();

    math::vec4f light_color [2] = {math::vec4f (1.0f, 1.0f, 1.0f, 1.0f), math::vec4f (0.0f, 0.0f, 0.2f, 1.0f)};
    math::vec4f light_dir [2] = {math::vec4f (0, 0, -1, 0), math::vec4f (0, -1, 0, 0)};
    
    frame_properties.SetProperty ("ProjectionMatrix", get_ortho_proj (-10, 10, -10, 10, -1000, 1000));
    frame_properties.SetProperty ("ViewMatrix", inverse (math::lookat (math::vec3f (0, 400, 0), math::vec3f (0.0f), math::vec3f (0, 0, 1))));
    frame_properties.SetProperty ("DirectLightColor", 2, light_color);
    frame_properties.SetProperty ("DirectLightWorldDirection", 2, light_dir);
    
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
