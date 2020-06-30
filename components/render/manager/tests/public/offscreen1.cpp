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
//    static size_t start = common::milliseconds ();

    static size_t last_fps = 0;
    static size_t frames_count = 0;

//    float t = float (common::milliseconds () - start) / 1000.f;

    if (common::milliseconds () - last_fps > 1000)
    {
      printf ("FPS: %.2f\n", float (frames_count)/float (common::milliseconds () - last_fps)*1000.f);
      fflush (stdout);

      last_fps = common::milliseconds ();
      frames_count = 0;        
      
      return;
    }    
    
    frames_count++;

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
  printf ("Results of offscreen1_test:\n");
  
  try
  {
    Test test (L"Offscreen 1", true);

    RenderManager render_manager = test.RenderManager ();
        
    render_manager.LoadResource ("data/offscreen.rfx");
    render_manager.LoadResource ("data/bottom.jpg");
    render_manager.LoadResource ("data/offscreen.xmtl");
    
    Entity     entity_offscreen    = render_manager.CreateEntity ();
    Entity     entity_onscreen     = render_manager.CreateEntity ();
    Primitive  primitive_offscreen = render_manager.CreatePrimitive ();
    Primitive  primitive_onscreen  = render_manager.CreatePrimitive ();
    SpriteList sprites_offscreen   = primitive_offscreen.AddBatchingSpriteList (SpriteMode_Oriented, math::vec3f (0, 1.0f, 0));
    SpriteList sprites_onscreen    = primitive_onscreen.AddBatchingSpriteList (SpriteMode_Oriented, math::vec3f (0, 1.0f, 0));
    Sprite     sprite;

    primitive_offscreen.Buffers ().ReserveDynamicBuffers (8192, 8192);
    primitive_onscreen.Buffers ().ReserveDynamicBuffers (8192, 8192);

    sprite.position   = math::vec3f (0.0f);
    sprite.size       = math::vec2f (1.0f);
    sprite.color      = math::vec4f (1.0f);
    sprite.tex_offset = math::vec2f (0.0f);
    sprite.tex_size   = math::vec2f (1.0f);
    sprite.normal     = math::vec3f (0, 0, 1.0f);
    sprite.rotation   = math::anglef ();

    sprites_offscreen.SetMaterial ("offscreen_material");
    sprites_offscreen.Add (1, &sprite);

    sprites_onscreen.SetMaterial ("onscreen_material");
    sprites_onscreen.Add (1, &sprite);

    entity_offscreen.SetPrimitive (primitive_offscreen);

    entity_offscreen.SetWorldMatrix (math::rotate (math::degree (45.0f), math::vec3f (0, 0, 1)));

    entity_onscreen.SetPrimitive (primitive_onscreen);

    Texture color_map (render_manager.CreateTexture ("offscreen_color"));
    Texture depth_map (render_manager.CreateTexture ("offscreen_depth_stencil"));

    Frame frame = render_manager.CreateFrame ();

    frame.DisableAutoCleanup ();
    frame.SetRenderTarget ("main_color_target", test.Window ().ColorBuffer ());
    frame.SetRenderTarget ("main_depth_stencil_target", test.Window ().DepthStencilBuffer ());
    frame.SetRenderTarget ("offscreen_color_target", color_map.RenderTarget (0, 0));
    frame.SetRenderTarget ("offscreen_depth_stencil_target", depth_map.RenderTarget (0, 0));
    frame.SetEffect ("main");
    frame.SetClearColor (math::vec4f (0.0f, 0.0f, 1.0f, 1.0f));

    frame.SetLocalTexture ("offscreen_color", color_map);

    common::PropertyMap entity_dependent_properties;

    frame.SetEntityDependentProperties (entity_dependent_properties);

    entity_dependent_properties.SetProperty ("myObjectMatrix", math::mat4f (1.0f));

    common::PropertyMap frame_properties = frame.Properties ();
    
    frame_properties.SetProperty ("myProjMatrix", get_ortho_proj (-0.5, 0.5, -0.5, 0.5, -15, 15));
    frame_properties.SetProperty ("myViewMatrix", inverse (math::lookat (math::vec3f (0, 0, 10), math::vec3f (0.0f), math::vec3f (0, 1, 0))));
    frame_properties.SetProperty ("ModelViewProjectionMatrix", frame_properties.GetMatrix ("myProjMatrix") * frame_properties.GetMatrix ("myViewMatrix"));
    
    frame.AddEntity (entity_offscreen);
    frame.AddEntity (entity_onscreen);
    
    test.ShowWindow ();
    
    syslib::Application::RegisterEventHandler (syslib::ApplicationEvent_OnIdle, xtl::bind (&idle, xtl::ref (test), xtl::ref (entity_offscreen), xtl::ref (frame)));
            
    return test.Run ();
  }
  catch (std::exception& e)
  {
    printf ("%s\n", e.what ());
  }

  return 0;
}

