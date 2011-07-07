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

int main ()
{
  printf ("Results of model_load_ffp_test:\n");
  
  try
  {
    common::LogFilter log_filter ("render.manager*", &log_print);    
    
    Test test (L"Model load FFP", true);

    RenderManager render_manager = test.RenderManager ();

    render_manager.LoadResource ("data/spy.xmesh");
    render_manager.LoadResource ("data/spy.xmtl");
    render_manager.LoadResource ("data/spy.rfx");
    
    Entity entity = render_manager.CreateEntity ();
    
    entity.SetPrimitive ("spy_body");
    
    Frame frame = render_manager.CreateFrame ();
    
    RectArea viewport;
    
    viewport.SetRect (0, 0, 400, 300);
        
    frame.SetRenderTarget ("main_color_target", test.Window ().ColorBuffer (), viewport);
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
    
    frame.Draw ();
    
    test.Window ().SwapBuffers ();
        
    return test.Run ();
  }
  catch (std::exception& e)
  {
    printf ("%s\n", e.what ());
  }

  return 0;
}
