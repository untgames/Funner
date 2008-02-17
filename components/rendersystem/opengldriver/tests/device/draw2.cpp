#include "shared.h"

const char* PIXEL_SHADER_FILE_NAME  = "data/glsl/wood.frag";
const char* VERTEX_SHADER_FILE_NAME = "data/glsl/wood.vert";

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

#pragma pack (1)

struct MyShaderParameters
{
  float        grain_size_recip;
  math::vec3f  dark_color;
  math::vec3f  color_spread;
  math::vec3f  light_position;
  float        scale;
  math::mat4f  transform;
};

stl::string read_shader (const char* file_name)
{
  common::InputFile file (file_name);
  
  stl::string buffer (file.Size (), ' ');
  
  file.Read (&buffer [0], file.Size ());
  
  return buffer;
} 

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

void idle (Test& test)
{
  static const   float DT = 0.01f;
  static float   t = 0;
  static clock_t last = 0;

  if (clock () - last < CLK_TCK / 30)
  {
    last = clock ();
    return;
  }

  MyShaderParameters my_shader_parameters;
  
  IBuffer* cb = test.device->SSGetConstantBuffer (0);
  
  if (!cb)
  {
    printf ("Null constant buffer #0\n");
    return;
  }
  
  cb->GetData (0, sizeof my_shader_parameters, &my_shader_parameters);
    
  my_shader_parameters.transform *= math::rotatef (math::deg2rad (.3f), 0, 0, 1);

  cb->SetData (0, sizeof my_shader_parameters, &my_shader_parameters);
  
  test.window.Invalidate ();
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
    Test test (L"OpenGL device test window (draw2)", "check_gl_errors=0");
    
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
    
    stl::string pixel_shader_source  = read_shader (PIXEL_SHADER_FILE_NAME),
                vertex_shader_source = read_shader (VERTEX_SHADER_FILE_NAME);
    
    ShaderDesc shader_descs [] = {
      {"p_shader", size_t (-1), pixel_shader_source.c_str (), "glsl.ps", ""},
      {"v_shader", size_t (-1), vertex_shader_source.c_str (), "glsl.vs", ""}
    };

    static ShaderParameter shader_parameters[] = {
      {"GrainSizeRecip", ShaderParameterType_Float, 0, offsetof (MyShaderParameters, grain_size_recip)},
      {"DarkColor", ShaderParameterType_Float3, 0, offsetof (MyShaderParameters, dark_color)},
      {"colorSpread", ShaderParameterType_Float3, 0, offsetof (MyShaderParameters, color_spread)},
      {"LightPosition", ShaderParameterType_Float3, 0, offsetof (MyShaderParameters, light_position)},
      {"Scale", ShaderParameterType_Float, 0, offsetof (MyShaderParameters, scale)},
      {"Transform", ShaderParameterType_Float4x4, 0, offsetof (MyShaderParameters, transform)}
    };
    
    ShaderParametersLayoutDesc shader_parameters_layout_desc = {sizeof shader_parameters / sizeof *shader_parameters, shader_parameters};

    ProgramPtr shader (test.device->CreateProgram (sizeof shader_descs / sizeof *shader_descs, shader_descs, &print));
    ShaderParametersLayoutPtr shader_parameters_layout (test.device->CreateShaderParametersLayout (shader_parameters_layout_desc));

    BufferDesc cb_desc;
    
    memset (&cb_desc, 0, sizeof cb_desc);
    
    cb_desc.size         = sizeof MyShaderParameters;
    cb_desc.usage_mode   = UsageMode_Default;
    cb_desc.bind_flags   = BindFlag_ConstantBuffer;
    cb_desc.access_flags = AccessFlag_ReadWrite;
    
    BufferPtr cb (test.device->CreateBuffer (cb_desc), false);

    MyShaderParameters my_shader_parameters = {
      1.0f,
      math::vec3f (0.6f, 0.3f, 0.1f),
      math::vec3f (0.15f, 0.15f / 2.0f, 0),
      math::vec3f (0.0f, 0.0f, 4.0f),
      1.0f
    };

    cb->SetData (0, sizeof my_shader_parameters, &my_shader_parameters);

    test.device->SSSetProgram (shader.get ());
    test.device->SSSetShaderParametersLayout (shader_parameters_layout.get ());
    test.device->SSSetConstantBuffer (0, cb.get ());

    printf ("Register callbacks\n");
    
    test.window.RegisterEventHandler (syslib::WindowEvent_OnPaint, xtl::bind (&redraw, xtl::ref (test)));
    test.window.RegisterEventHandler (syslib::WindowEvent_OnSize, xtl::bind (&resize, xtl::ref (test)));
    test.window.RegisterEventHandler (syslib::WindowEvent_OnClose, xtl::bind (&close));
    syslib::Application::RegisterEventHandler (syslib::ApplicationEvent_OnIdle, xtl::bind (&idle, xtl::ref (test)));

    printf ("Main loop\n");

    syslib::Application::Run ();
  }
  catch (std::exception& e)
  {
    printf ("exception: %s\n", e.what ());
  }

  return 0;
}
