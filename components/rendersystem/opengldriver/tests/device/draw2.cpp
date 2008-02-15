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

struct MyShaderParameters
{
  float time [4];
};

const char* VERTEX_SHADER_SOURCE = 
"uniform vec4 time;\n"
"varying vec4 color;\n"

"void main(void)\n"
"{\n"
"color         = gl_Color * time;\n"
"gl_Position   = ftransform ();\n"
"}";

const char* PIXEL_SHADER_SOURCE = 
"varying vec4 color;\n"
"void main(void)\n"
"{\n"
"gl_FragColor = color;\n"
"}";

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

void print (const char* message)
{
  printf ("Shader message: '%s'\n", message);
}

int main ()
{
  printf ("Results of draw2_test:\n");
  
  try
  {
    Test test (L"OpenGL device test window (draw2)");
    
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

    printf ("Set shader stage\n");

    ShaderDesc shader_descs[] = {
      {"p_shader", strlen (PIXEL_SHADER_SOURCE), PIXEL_SHADER_SOURCE, "glsl.ps", ""},
      {"v_shader", strlen (VERTEX_SHADER_SOURCE), VERTEX_SHADER_SOURCE, "glsl.vs", ""}
    };

    static ShaderParameter shader_parameters[] = {
      {"time", ShaderParameterType_Float4, 0, offsetof (MyShaderParameters, time)}
    };

    ShaderParametersLayoutDesc shader_parameters_layout_desc = {sizeof shader_parameters / sizeof *shader_parameters, shader_parameters};

    ShaderPtr shader (test.device->CreateShader (sizeof shader_descs / sizeof *shader_descs, shader_descs, &print));
    ShaderParametersLayoutPtr shader_parameters_layout (test.device->CreateShaderParametersLayout (shader_parameters_layout_desc));

    BufferDesc cb_desc;
    
    memset (&cb_desc, 0, sizeof cb_desc);
    
    cb_desc.size         = sizeof MyShaderParameters;
    cb_desc.usage_mode   = UsageMode_Default;
    cb_desc.bind_flags   = BindFlag_ConstantBuffer;
    cb_desc.access_flags = AccessFlag_ReadWrite;
    
    BufferPtr cb (test.device->CreateBuffer (cb_desc), false);

    MyShaderParameters my_shader_parameters[] = {{1.f, 0.5f, 0.25f, 0.1f}};

    cb->SetData (0, sizeof my_shader_parameters, &my_shader_parameters);

    test.device->SSSetShader (shader.get ());
    test.device->SSSetShaderParametersLayout (shader_parameters_layout.get ());
    test.device->SSSetConstantBuffer (0, cb.get ());

    printf ("Register callbacks\n");
    
    test.window.RegisterEventHandler (syslib::WindowEvent_OnPaint, xtl::bind (&redraw, xtl::ref (test)));
    test.window.RegisterEventHandler (syslib::WindowEvent_OnSize, xtl::bind (&resize, xtl::ref (test)));
    test.window.RegisterEventHandler (syslib::WindowEvent_OnClose, xtl::bind (&close));

    printf ("Main loop\n");

    syslib::Application::Run ();
  }
  catch (std::exception& e)
  {
    printf ("exception: %s\n", e.what ());
  }

  return 0;
}
