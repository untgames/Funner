#include "shared.h"

struct Vec3f
{
  float x, y, z;
};

struct Color4ub
{
  unsigned char red, green, blue, alpha;
};

struct MyVertex
{
  Vec3f    position;
  Vec3f    normal;
  Color4ub color;
};

struct MyShaderParameters2
{
  math::mat4f  transform;
};

ViewPtr create_depth_stencil_view (IDevice* device, ISwapChain* swap_chain)
{
  TexturePtr depth_stencil_texture (device->CreateDepthStencilTexture (swap_chain), false);

  ViewDesc depth_stencil_view_desc;

  memset (&depth_stencil_view_desc, 0, sizeof (depth_stencil_view_desc));

  return ViewPtr (device->CreateView (depth_stencil_texture.get (), depth_stencil_view_desc), false);
}

ViewPtr create_render_view (IDevice* device, ISwapChain* swap_chain)
{
  TexturePtr render_target_texture (device->CreateRenderTargetTexture (swap_chain, 1), false);

  ViewDesc render_target_view_desc;

  memset (&render_target_view_desc, 0, sizeof (render_target_view_desc));

  return ViewPtr (device->CreateView (render_target_texture.get (), render_target_view_desc), false);
}

void print (const char* message)
{
  printf ("Shader message: '%s'\n", message);
}

class WindowResizeApplication
{
  TestPtr              test;
  InputLayoutPtr       layout;
  BufferPtr            vb;
  DepthStencilStatePtr depth_stencil_state;

  static void Redraw (Test& test)
  {
    test.device->GetImmediateContext ()->Draw (PrimitiveType_TriangleList, 0, 3);
  }

  void OnInitialize ()
  {
    try
    {
      test = new Test(L"OpenGL device test window (window_resize)", &Redraw);

      test->window.Show ();

      CreateVertexBuffer ();
      SetInputStage ();
      SetOutputStage ();
    }
    catch (const xtl::exception& e)
    {
      printf("%s failed: %s\n", __FUNCTION__, e.what());
      syslib::Application::Exit (1);
    }
  }

  void OnExit ()
  {
    // Test object should be destroyed
    // BEFORE application exit.
    test.reset ();
  }

  void CreateVertexBuffer ()
  {
    try
    {
      printf ("Create vertex buffer\n");
      
      static const MyVertex verts [] = {
        {{-1, -1, 0}, {0, 0, 1}, {255, 0, 0, 0}},
        {{ 1, -1, 0}, {0, 0, 1}, {0, 255, 0, 0}},
        {{ 0, 1, 0}, {0, 0, 1}, {0, 0, 255, 0}},
      };    
      
      static const size_t VERTICES_COUNT = sizeof verts / sizeof *verts;

      BufferDesc vb_desc;

      memset (&vb_desc, 0, sizeof vb_desc);

      vb_desc.size         = sizeof (MyVertex)* VERTICES_COUNT;
      vb_desc.usage_mode   = UsageMode_Default;
      vb_desc.bind_flags   = BindFlag_VertexBuffer;
      vb_desc.access_flags = AccessFlag_ReadWrite;

      vb = BufferPtr (test->device->CreateBuffer (vb_desc), false);    

      vb->SetData (0, vb_desc.size, verts);
    }
    catch (xtl::exception& e)
    {
      e.touch (__FUNCTION__);
      throw;
    }
  }

  void SetInputStage ()
  {
    try
    {
      printf ("Set input-stage\n");

      VertexAttribute attributes [] = {
        {
            test->device->GetVertexAttributeSemanticName (VertexAttributeSemantic_Normal)
          , InputDataFormat_Vector3
          , InputDataType_Float
          , 0
          , offsetof (MyVertex, normal)
          , sizeof (MyVertex)
        },
        {
            test->device->GetVertexAttributeSemanticName (VertexAttributeSemantic_Position)
          , InputDataFormat_Vector3
          , InputDataType_Float
          , 0
          , offsetof (MyVertex, position)
          , sizeof (MyVertex)
        },
        {
            test->device->GetVertexAttributeSemanticName (VertexAttributeSemantic_Color)
          , InputDataFormat_Vector4
          , InputDataType_Float
          , 0, offsetof (MyVertex, color)
          , sizeof (MyVertex)
        },
      };
      
      InputLayoutDesc layout_desc;
      
      memset (&layout_desc, 0, sizeof layout_desc);
      
      layout_desc.vertex_attributes_count = sizeof attributes / sizeof *attributes;
      layout_desc.vertex_attributes       = attributes;
      layout_desc.index_type              = InputDataType_UInt;
      layout_desc.index_buffer_offset     = 0;
      
      layout = InputLayoutPtr (test->device->CreateInputLayout (layout_desc), false);

      test->device->GetImmediateContext ()->ISSetInputLayout (layout.get ());
      test->device->GetImmediateContext ()->ISSetVertexBuffer (0, vb.get ());
    }
    catch (xtl::exception& e)
    {
      e.touch (__FUNCTION__);
      throw;
    }
  }

  void SetOutputStage()
  {
    try
    {
      printf ("Set output stage\n");
     
      DepthStencilDesc depth_stencil_desc;

      memset (&depth_stencil_desc, 0, sizeof (depth_stencil_desc));

      depth_stencil_desc.depth_test_enable   = true;
      depth_stencil_desc.stencil_test_enable = false;
      depth_stencil_desc.depth_compare_mode  = CompareMode_Less;
      depth_stencil_desc.depth_write_enable  = false;

      depth_stencil_state = DepthStencilStatePtr (test->device->CreateDepthStencilState (depth_stencil_desc), false);

      test->device->GetImmediateContext ()->OSSetDepthStencilState (depth_stencil_state.get ());
    }
    catch (xtl::exception& e)
    {
      e.touch (__FUNCTION__);
      throw;
    }
  }

public:
  WindowResizeApplication ()
  {
    syslib::Application::RegisterEventHandler (
        syslib::ApplicationEvent_OnInitialize
      , xtl::bind(&WindowResizeApplication::OnInitialize, this)
    );

    syslib::Application::RegisterEventHandler (
        syslib::ApplicationEvent_OnExit
      , xtl::bind(&WindowResizeApplication::OnExit, this)
    );
  }

  void Run ()
  {
    printf ("Main loop\n");

    syslib::Application::Run ();
  }
};

int main ()
{
  printf ("Results of window_resize_test:\n");
  
  try
  {
    WindowResizeApplication app;

    app.Run ();
  }
  catch (std::exception& e)
  {
    printf ("exception: %s\n", e.what ());
    return 1;
  }

  return 0;
}
