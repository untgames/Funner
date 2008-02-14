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

void resize (Test& test)
{
  try
  {
    Viewport vp;

    vp.x         = 0;
    vp.y         = 0;
    vp.width     = test.window.Width ();
    vp.height    = test.window.Height ();
    vp.min_depth = 0;
    vp.max_depth = 1;

    test.device->RSSetViewport (vp);
    
    test.window.Invalidate ();
  }
  catch (std::exception& e)
  {
    printf ("resize exception: %s\n", e.what ());
  }
}

void redraw (Test& test)
{
  try
  {
    Color4f clear_color;

    clear_color.red   = 0;
    clear_color.green = 0.7f;
    clear_color.blue  = 0.7f;
    clear_color.alpha = 0;

    test.device->ClearViews (ClearFlag_All, clear_color, 1.0f, 0);
    test.device->Draw (PrimitiveType_TriangleList, 0, 3);
   
    test.swap_chain->Present ();    
  }
  catch (std::exception& e)
  {
    printf ("redraw exception: %s\n", e.what ());
  }
}

void close ()
{
  syslib::Application::Exit (0);
}

int main ()
{
  printf ("Results of draw1_test:\n");
  
  try
  {
    Test test (L"OpenGL device test window (draw1)");
    
    test.window.Show ();
   
    printf ("Create vertex buffer\n");
    
    static const size_t VERTICES_COUNT = 3;
    
    BufferDesc vb_desc;
    
    memset (&vb_desc, 0, sizeof vb_desc);
    
    vb_desc.size         = sizeof MyVertex * VERTICES_COUNT;
    vb_desc.usage_mode   = UsageMode_Default;
    vb_desc.bind_flags   = BindFlag_VertexBuffer;
    vb_desc.access_flags = AccessFlag_Read | AccessFlag_Write;
    
    BufferPtr vb (test.device->CreateBuffer (vb_desc), false);
    
    static const MyVertex verts [] = {
      {{-1, -1, 0}, {0, 0, 1}, {255, 0, 0, 0}},
      {{ 1, -1, 0}, {0, 0, 1}, {0, 255, 0, 0}},
      {{ 0, 1, 0}, {0, 0, 1}, {0, 0, 255, 0}},
    };
    
    vb->SetData (0, vb_desc.size, verts);
    
    printf ("Set input-stage\n");
    
    VertexAttribute attributes [] = {
      {VertexAttributeSemantic_Normal, InputDataFormat_Vector3, InputDataType_Float, 0, offsetof (MyVertex, normal), sizeof (MyVertex)},
      {VertexAttributeSemantic_Position, InputDataFormat_Vector3, InputDataType_Float, 0, offsetof (MyVertex, position), sizeof (MyVertex)},
      {VertexAttributeSemantic_Color, InputDataFormat_Vector4, InputDataType_Float, 0, offsetof (MyVertex, color), sizeof (MyVertex)},
    };
    
    InputLayoutDesc layout_desc;
    
    memset (&layout_desc, 0, sizeof layout_desc);
    
    layout_desc.vertex_attributes_count = sizeof attributes / sizeof *attributes;
    layout_desc.vertex_attributes       = attributes;
    layout_desc.index_type              = InputDataType_UInt;
    layout_desc.index_buffer_offset     = 0;
    
    InputLayoutPtr layout (test.device->CreateInputLayout (layout_desc), false);

    test.device->ISSetInputLayout (layout.get ());
    test.device->ISSetVertexBuffer (0, vb.get ());

    printf ("Register callbacks\n");
    
    test.window.RegisterEventHandler (syslib::WindowEvent_OnPaint, xtl::bind (&redraw, xtl::ref (test)));
    test.window.RegisterEventHandler (syslib::WindowEvent_OnSize, xtl::bind (&resize, xtl::ref (test)));
    test.window.RegisterEventHandler (syslib::WindowEvent_OnClose, xtl::bind (&close));

    printf ("Main loop");

    syslib::Application::Run ();
  }
  catch (std::exception& e)
  {
    printf ("exception: %s\n", e.what ());
  }

  return 0;
}
