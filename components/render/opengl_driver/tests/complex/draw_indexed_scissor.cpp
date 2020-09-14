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

class DrawIndexedApplication
{
  TestPtr        test;
  BufferPtr      vb;
  BufferPtr      ib;
  InputLayoutPtr layout;

  static void Redraw (Test& test)
  {
    RasterizerDesc rasterizer_state_desc;

    rasterizer_state_desc.fill_mode = FillMode_Solid;
    rasterizer_state_desc.cull_mode = CullMode_None;
    rasterizer_state_desc.front_counter_clockwise = true;
    rasterizer_state_desc.depth_bias = 0;
    rasterizer_state_desc.scissor_enable = true;
    rasterizer_state_desc.multisample_enable = false;
    rasterizer_state_desc.antialiased_line_enable = false;

    xtl::com_ptr<IRasterizerState> rasterizer_state = test.device->CreateRasterizerState (rasterizer_state_desc);

    test.device->GetImmediateContext ()->RSSetState (rasterizer_state.get ());

    Rect scissor_rect { 0, 0, 1000, 4000 };

    test.device->GetImmediateContext ()->RSSetScissor (0, scissor_rect);
    test.device->GetImmediateContext ()->DrawIndexed (PrimitiveType_TriangleList, 0, 3, 0);

    unsigned int render_target_index = 0;
    Color4f      clear_color { 1.f, 0.f, 0.f, 1.f };

    Viewport viewport;

    viewport.x         = 300;
    viewport.y         = 100;
    viewport.width     = 100;
    viewport.height    = 100;
    viewport.min_depth = 0.f;
    viewport.max_depth = 1.f;

    test.device->GetImmediateContext ()->RSSetViewport (0, viewport);
    test.device->GetImmediateContext ()->ClearViews (ClearFlag_All | ClearFlag_ViewportOnly, 1, &render_target_index, &clear_color, 0.0, 0);
  }

  void OnInitialize ()
  {
    try
    {
      test = new Test(L"OpenGL device test window (draw_indexed)", &Redraw);

      test->window.Show ();

      CreateVertexBuffer ();
      CreateIndexBuffer ();
      SetInputStage ();
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

      static const size_t VERTICES_COUNT = 3;

      BufferDesc vb_desc;

      memset (&vb_desc, 0, sizeof vb_desc);

      vb_desc.size         = sizeof (MyVertex) * VERTICES_COUNT;
      vb_desc.usage_mode   = UsageMode_Default;
      vb_desc.bind_flags   = BindFlag_VertexBuffer;
      vb_desc.access_flags = AccessFlag_Read | AccessFlag_Write;

      vb = BufferPtr (test->device->CreateBuffer (vb_desc), false);

      static const MyVertex verts [] = {
        {{-1, -1, 0}, {0, 0, 1}, {255, 0, 0, 0}},
        {{ 1, -1, 0}, {0, 0, 1}, {0, 255, 0, 0}},
        {{ 0, 1, 0}, {0, 0, 1}, {0, 0, 255, 0}},
      };

      vb->SetData (0, vb_desc.size, verts);
    }
    catch (xtl::exception& e)
    {
      e.touch (__FUNCTION__);
      throw;
    }
  }

  void CreateIndexBuffer()
  {
    try
    {
      printf ("Create index buffer\n");

      static unsigned short indices [] = {0, 1, 2};

      BufferDesc ib_desc;

      memset (&ib_desc, 0, sizeof ib_desc);

      ib_desc.size         = sizeof indices;
      ib_desc.usage_mode   = UsageMode_Default;
      ib_desc.bind_flags   = BindFlag_IndexBuffer;
      ib_desc.access_flags = AccessFlag_ReadWrite;

      ib = BufferPtr (test->device->CreateBuffer (ib_desc), false);

      ib->SetData (0, ib_desc.size, indices);
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
          , InputDataType_UByte
          , 0
          , offsetof (MyVertex, color)
          , sizeof (MyVertex)
        },
      };

      InputLayoutDesc layout_desc;

      memset (&layout_desc, 0, sizeof layout_desc);

      layout_desc.vertex_attributes_count = sizeof attributes / sizeof *attributes;
      layout_desc.vertex_attributes       = attributes;
      layout_desc.index_type              = InputDataType_UShort;
      layout_desc.index_buffer_offset     = 0;

      layout = InputLayoutPtr (test->device->CreateInputLayout (layout_desc), false);

      test->device->GetImmediateContext ()->ISSetInputLayout (layout.get ());
      test->device->GetImmediateContext ()->ISSetVertexBuffer (0, vb.get ());
      test->device->GetImmediateContext ()->ISSetIndexBuffer (ib.get ());
    }
    catch (xtl::exception& e)
    {
      e.touch (__FUNCTION__);
      throw;
    }
  }

public:
  DrawIndexedApplication ()
  {
    syslib::Application::RegisterEventHandler (
        syslib::ApplicationEvent_OnInitialize
      , xtl::bind(&DrawIndexedApplication::OnInitialize, this)
    );

    syslib::Application::RegisterEventHandler (
        syslib::ApplicationEvent_OnExit
      , xtl::bind(&DrawIndexedApplication::OnExit, this)
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
  printf ("Results of draw_indexed_test:\n");

  try
  {
    DrawIndexedApplication app;

    app.Run ();
  }
  catch (std::exception& e)
  {
    printf ("exception: %s\n", e.what ());
    return 1;
  }

  return 0;
}
