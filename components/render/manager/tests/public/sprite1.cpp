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
  
  math::mat4f model_view_tm = frame.Properties ().GetMatrix ("myViewMatrix") * entity.WorldMatrix ();  

  out_params.mvp_matrix = frame.Properties ().GetMatrix ("myProjMatrix") * model_view_tm;

  properties.SetProperty ("myObjectMatrix", entity.WorldMatrix ());
}

void idle (Test& test, Entity& entity, Frame& frame)
{
  try
  {
    static size_t start = common::milliseconds ();

    static size_t last_fps = 0;
    static size_t frames_count = 0;

    float t = float (common::milliseconds () - start) / 1000.f;

    if (common::milliseconds () - last_fps > 1000)
    {
      printf ("FPS: %.2f\n", float (frames_count)/float (common::milliseconds () - last_fps)*1000.f);
      fflush (stdout);

      last_fps = common::milliseconds ();
      frames_count = 0;        
      
      return;
    }    
    
    frames_count++;

    common::PropertyMap entity_dependent_properties = frame.EntityDependentProperties ();

    entity_dependent_properties.SetProperty ("myObjectMatrix", math::mat4f (1.0f));
    
    common::PropertyMap frame_properties = frame.Properties ();
    common::PropertyMap entity_properties = entity.Properties ();  

    float angle = t * 3.1415926f / 4.0f;
    
////    entity.SetWorldMatrix (math::rotate (math::radian (angle), math::vec3f (0, 0, 1)));
//    entity.SetWorldMatrix (math::rotate (math::radian (angle), math::vec3f (0, 0, 1)) *
//      math::rotate (math::radian (angle*0.2f), math::vec3f (1, 0, 0)));

    frame.Draw ();
      
    test.Window ().SwapBuffers ();
  }
  catch (std::exception& e)
  {
    printf ("exception: %s\n    at idle\n", e.what ());
  }
  catch (...)
  {
    printf ("unknown exception\n    at idle\n");
  }
}

int main ()
{
  printf ("Results of sprite1_test:\n");
  
  try
  {
    Test test (L"Sprite 1", true);

    RenderManager render_manager = test.RenderManager ();
        
    render_manager.LoadResource ("data/sprites.rfx");    
    render_manager.LoadResource ("data/bottom.jpg");
    render_manager.LoadResource ("data/sprites.xmtl");
    
    Entity     entity    = render_manager.CreateEntity ();
    Primitive  primitive = render_manager.CreatePrimitive ();
//    SpriteList sprites   = primitive.AddStandaloneSpriteList (SpriteMode_Oriented, math::vec3f (0, 1.0f, 0));
//    SpriteList sprites   = primitive.AddStandaloneSpriteList (SpriteMode_Billboard, math::vec3f (0, 1.0f, 0));
//    SpriteList sprites   = primitive.AddBatchingSpriteList (SpriteMode_Oriented, math::vec3f (0, 1.0f, 0));
    SpriteList sprites   = primitive.AddBatchingSpriteList (SpriteMode_Oriented, math::vec3f (0, 1.0f, 0));
    Sprite     sprite;

    primitive.Buffers ().ReserveDynamicBuffers (8192, 8192);

    sprite.position   = math::vec3f (0.0f);
    sprite.size       = math::vec2f (1.0f);
    sprite.color      = math::vec4f (1.0f);
    sprite.tex_offset = math::vec2f (0.0f);
    sprite.tex_size   = math::vec2f (1.0f);
    sprite.normal     = math::vec3f (0, 0, 1.0f);
    sprite.rotation   = math::anglef ();

    sprites.SetMaterial ("sprite");
    sprites.Add (1, &sprite);

    entity.SetPrimitive (primitive);

    entity.SetWorldMatrix (math::rotate (math::degree (45.0f), math::vec3f (0, 0, 1)));
    
    Frame frame = render_manager.CreateFrame ();

    frame.DisableAutoCleanup ();
    frame.SetRenderTarget ("main_color_target", test.Window ().ColorBuffer ());
    frame.SetRenderTarget ("main_depth_stencil_target", test.Window ().DepthStencilBuffer ());
    frame.SetEffect ("main");
    frame.SetClearColor (math::vec4f (0.0f, 0.0f, 1.0f, 1.0f));
    frame.SetEntityDrawHandler (&update_entity_frame_transformations);

    common::PropertyMap entity_dependent_properties;

    frame.SetEntityDependentProperties (entity_dependent_properties);

    entity_dependent_properties.SetProperty ("myObjectMatrix", math::mat4f (1.0f));
    
    common::PropertyMap frame_properties = frame.Properties ();
    common::PropertyMap entity_properties = entity.Properties ();
    
    frame_properties.SetProperty ("myProjMatrix", get_ortho_proj (-2, 2, -2, 2, -15, 15));
    frame_properties.SetProperty ("myViewMatrix", inverse (math::lookat (math::vec3f (0, 0, 10), math::vec3f (0.0f), math::vec3f (0, 1, 0))));

    frame.SetViewProjectionMatrix (frame_properties.GetMatrix ("myProjMatrix") * frame_properties.GetMatrix ("myViewMatrix"));
    
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

